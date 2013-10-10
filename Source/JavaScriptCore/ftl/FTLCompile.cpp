/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "FTLCompile.h"

#if ENABLE(FTL_JIT)

#include "CodeBlockWithJITType.h"
#include "CCallHelpers.h"
#include "DFGCommon.h"
#include "DataView.h"
#include "Disassembler.h"
#include "FTLExitThunkGenerator.h"
#include "FTLJITCode.h"
#include "FTLThunks.h"
#include "JITStubs.h"
#include "LinkBuffer.h"
#include "RepatchBuffer.h"
#include <wtf/LLVMHeaders.h>

namespace JSC { namespace FTL {

using namespace DFG;

static uint8_t* mmAllocateCodeSection(
    void* opaqueState, uintptr_t size, unsigned alignment, unsigned, const char* sectionName)
{
    
    State& state = *static_cast<State*>(opaqueState);
    
    RELEASE_ASSERT(alignment <= jitAllocationGranule);
    
    RefPtr<ExecutableMemoryHandle> result =
        state.graph.m_vm.executableAllocator.allocate(
            state.graph.m_vm, size, state.graph.m_codeBlock, JITCompilationMustSucceed);
    
    state.jitCode->addHandle(result);
    state.codeSectionNames.append(sectionName);
    
    return static_cast<uint8_t*>(result->start());
}

static uint8_t* mmAllocateDataSection(
    void* opaqueState, uintptr_t size, unsigned alignment, unsigned sectionID,
    const char* sectionName, LLVMBool isReadOnly)
{
    UNUSED_PARAM(sectionID);
    UNUSED_PARAM(isReadOnly);

    State& state = *static_cast<State*>(opaqueState);
    
    RELEASE_ASSERT(alignment <= sizeof(LSectionWord));
    
    RefCountedArray<LSectionWord> section(
        (size + sizeof(LSectionWord) - 1) / sizeof(LSectionWord));
    
    if (!strcmp(sectionName, "__js_stackmaps"))
        state.stackmapsSection = section;
    else {
        state.jitCode->addDataSection(section);
        state.dataSectionNames.append(sectionName);
    }
    
    return bitwise_cast<uint8_t*>(section.data());
}

static LLVMBool mmApplyPermissions(void*, char**)
{
    return false;
}

static void mmDestroy(void*)
{
}

static void dumpDataSection(RefCountedArray<LSectionWord> section, const char* prefix)
{
    for (unsigned j = 0; j < section.size(); ++j) {
        char buf[32];
        snprintf(buf, sizeof(buf), "0x%lx", static_cast<unsigned long>(bitwise_cast<uintptr_t>(section.data() + j)));
        dataLogF("%s%16s: 0x%016llx\n", prefix, buf, static_cast<long long>(section[j]));
    }
}

static void fixFunctionBasedOnStackMaps(
    State& state, CodeBlock* codeBlock, JITCode* jitCode, GeneratedFunction generatedFunction,
    StackMaps::RecordMap& recordMap)
{
    VM& vm = state.graph.m_vm;
    MacroAssemblerCodeRef osrExitThunk =
        vm.getCTIStub(osrExitGenerationThunkGenerator);
    CodeLocationLabel target = CodeLocationLabel(osrExitThunk.code());

    ExitThunkGenerator exitThunkGenerator(state);
    exitThunkGenerator.emitThunks();
    if (exitThunkGenerator.didThings()) {
        OwnPtr<LinkBuffer> linkBuffer = adoptPtr(new LinkBuffer(
            vm, &exitThunkGenerator, codeBlock, JITCompilationMustSucceed));
        
        ASSERT(state.osrExit.size() == state.jitCode->osrExit.size());
        
        for (unsigned i = 0; i < state.osrExit.size(); ++i) {
            OSRExitCompilationInfo& info = state.osrExit[i];
            OSRExit& exit = jitCode->osrExit[i];
            
            linkBuffer->link(info.m_thunkJump, target);
            
            info.m_thunkAddress = linkBuffer->locationOf(info.m_thunkLabel);
            
            exit.m_patchableCodeOffset = linkBuffer->offsetOf(info.m_thunkJump);
        }
        
        state.finalizer->initializeExitThunksLinkBuffer(linkBuffer.release());
    }

    RepatchBuffer repatchBuffer(codeBlock);
    
    for (unsigned exitIndex = jitCode->osrExit.size(); exitIndex--;) {
        OSRExitCompilationInfo& info = state.osrExit[exitIndex];
        OSRExit& exit = jitCode->osrExit[exitIndex];
        StackMaps::Record& record = recordMap.find(exit.m_stackmapID)->value;
        
        repatchBuffer.replaceWithJump(
            CodeLocationLabel(
                bitwise_cast<char*>(generatedFunction) + record.instructionOffset),
            info.m_thunkAddress);
    }
}

void compile(State& state)
{
    char* error = 0;
    
    LLVMMCJITCompilerOptions options;
    LLVMInitializeMCJITCompilerOptions(&options, sizeof(options));
    options.OptLevel = Options::llvmBackendOptimizationLevel();
    options.NoFramePointerElim = true;
    if (Options::useLLVMSmallCodeModel())
        options.CodeModel = LLVMCodeModelSmall;
    options.EnableFastISel = Options::enableLLVMFastISel();
    options.MCJMM = LLVMCreateSimpleMCJITMemoryManager(
        &state, mmAllocateCodeSection, mmAllocateDataSection, mmApplyPermissions, mmDestroy);
    
    LLVMExecutionEngineRef engine;
    
    if (LLVMCreateMCJITCompilerForModule(&engine, state.module, &options, sizeof(options), &error)) {
        dataLog("FATAL: Could not create LLVM execution engine: ", error, "\n");
        CRASH();
    }

    LLVMPassManagerBuilderRef passBuilder = LLVMPassManagerBuilderCreate();
    LLVMPassManagerBuilderSetOptLevel(passBuilder, Options::llvmOptimizationLevel());
    LLVMPassManagerBuilderSetSizeLevel(passBuilder, Options::llvmSizeLevel());
    
    LLVMPassManagerRef functionPasses = LLVMCreateFunctionPassManagerForModule(state.module);
    LLVMPassManagerRef modulePasses = LLVMCreatePassManager();
    
    LLVMAddTargetData(LLVMGetExecutionEngineTargetData(engine), modulePasses);
    
    LLVMPassManagerBuilderPopulateFunctionPassManager(passBuilder, functionPasses);
    LLVMPassManagerBuilderPopulateModulePassManager(passBuilder, modulePasses);
    
    LLVMPassManagerBuilderDispose(passBuilder);

    LLVMInitializeFunctionPassManager(functionPasses);
    for (LValue function = LLVMGetFirstFunction(state.module); function; function = LLVMGetNextFunction(function))
        LLVMRunFunctionPassManager(functionPasses, function);
    LLVMFinalizeFunctionPassManager(functionPasses);
    
    LLVMRunPassManager(modulePasses, state.module);

    if (DFG::shouldShowDisassembly() || DFG::verboseCompilationEnabled())
        state.dumpState("after optimization");
    
    // FIXME: Need to add support for the case where JIT memory allocation failed.
    // https://bugs.webkit.org/show_bug.cgi?id=113620
    state.generatedFunction = reinterpret_cast<GeneratedFunction>(LLVMGetPointerToGlobal(engine, state.function));
    LLVMDisposePassManager(functionPasses);
    LLVMDisposePassManager(modulePasses);
    LLVMDisposeExecutionEngine(engine);

    if (shouldShowDisassembly()) {
        for (unsigned i = 0; i < state.jitCode->handles().size(); ++i) {
            ExecutableMemoryHandle* handle = state.jitCode->handles()[i].get();
            dataLog(
                "Generated LLVM code for ",
                CodeBlockWithJITType(state.graph.m_codeBlock, JITCode::DFGJIT),
                " #", i, ", ", state.codeSectionNames[i], ":\n");
            disassemble(
                MacroAssemblerCodePtr(handle->start()), handle->sizeInBytes(),
                "    ", WTF::dataFile(), LLVMSubset);
        }
        
        for (unsigned i = 0; i < state.jitCode->dataSections().size(); ++i) {
            const RefCountedArray<LSectionWord>& section = state.jitCode->dataSections()[i];
            dataLog(
                "Generated LLVM data section for ",
                CodeBlockWithJITType(state.graph.m_codeBlock, JITCode::DFGJIT),
                " #", i, ", ", state.dataSectionNames[i], ":\n");
            dumpDataSection(section, "    ");
        }
    }
    
    if (state.stackmapsSection.size()) {
        if (shouldShowDisassembly()) {
            dataLog(
                "Generated LLVM stackmaps section for ",
                CodeBlockWithJITType(state.graph.m_codeBlock, JITCode::DFGJIT), ":\n");
            dataLog("    Raw data:\n");
            dumpDataSection(state.stackmapsSection, "    ");
        }
        
        RefPtr<DataView> stackmapsData = DataView::create(
            ArrayBuffer::create(state.stackmapsSection.data(), state.stackmapsSection.byteSize()));
        state.jitCode->stackmaps.parse(stackmapsData.get());
    
        if (shouldShowDisassembly()) {
            dataLog("    Structured data:\n");
            state.jitCode->stackmaps.dumpMultiline(WTF::dataFile(), "        ");
        }
        
        StackMaps::RecordMap recordMap = state.jitCode->stackmaps.getRecordMap();
        fixFunctionBasedOnStackMaps(
            state, state.graph.m_codeBlock, state.jitCode.get(), state.generatedFunction,
            recordMap);
        
        if (shouldShowDisassembly()) {
            for (unsigned i = 0; i < state.jitCode->handles().size(); ++i) {
                if (state.codeSectionNames[i] != "__text")
                    continue;
                
                ExecutableMemoryHandle* handle = state.jitCode->handles()[i].get();
                dataLog(
                    "Generated LLVM code after stackmap-based fix-up for ",
                    CodeBlockWithJITType(state.graph.m_codeBlock, JITCode::DFGJIT),
                    " #", i, ", ", state.codeSectionNames[i], ":\n");
                disassemble(
                    MacroAssemblerCodePtr(handle->start()), handle->sizeInBytes(),
                    "    ", WTF::dataFile(), LLVMSubset);
            }
        }
    }
    
    state.module = 0; // We no longer own the module.
}

} } // namespace JSC::FTL

#endif // ENABLE(FTL_JIT)

