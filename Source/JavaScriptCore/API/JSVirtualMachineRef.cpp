//
//  JSVirtualMachineRef.cpp
//  JavaScriptCore
//
//  Created by Matt Langston on 8/22/13.
//
//

#include "config.h"
#include "JSVirtualMachineRef.h"
#include "JSVirtualMachineRefPrivate.h"
#include "JSContextRef.h"
//#include <wtf/Forward.h>
#include <wtf/PassRefPtr.h>
#include <wtf/ThreadSafeRefCounted.h>

namespace Appcelerator {
    
    class JSVirtualMachine : public ThreadSafeRefCounted<JSVirtualMachine> {
    public:
        
        static PassRefPtr<JSVirtualMachine> createJSVirtualMachine();
        static PassRefPtr<JSVirtualMachine> createJSVirtualMachine(JSContextGroupRef);
        JS_EXPORT_PRIVATE ~JSVirtualMachine();
        
    private:
        JSVirtualMachine();
        JSVirtualMachine(JSContextGroupRef);
        
        JSContextGroupRef m_group;
        //        NSMapTable *m_contextCache;
        //        NSMapTable *m_externalObjectGraph;
    };
    
} // namespace Appcelerator


namespace Appcelerator {
    
    JSVirtualMachine::JSVirtualMachine() : JSVirtualMachine{JSContextGroupCreate()} {
        // The extra JSContextGroupRetain is balanced here.
        JSContextGroupRelease(m_group);
    }
    
    JSVirtualMachine::JSVirtualMachine(JSContextGroupRef group) : m_group(JSContextGroupRetain(group)) {
        // TODO: Initialize m_contextCache with:
        //
        // keys:   NSPointerFunctionsOpaqueMemory | NSPointerFunctionsOpaquePersonality
        // values: NSPointerFunctionsWeakMemory   | NSPointerFunctionsObjectPersonality
        
        // TODO: Initialize m_externalObjectGraph with:
        //
        // keys:   NSPointerFunctionsWeakMemory   | NSPointerFunctionsObjectPersonality
        // values: NSPointerFunctionsStrongMemory | NSPointerFunctionsObjectPersonality
    }
    
    JSVirtualMachine::~JSVirtualMachine() {
        JSContextGroupRelease(m_group);
    }

} // namespace Appcelerator

// TODO: This should go in APICast.h
inline Appcelerator::JSVirtualMachine* toJS(JSVirtualMachineRef virtualMachine)
{
    return reinterpret_cast<Appcelerator::JSVirtualMachine*>(const_cast<OpaqueJSVirtualMachine*>(virtualMachine));
}

// TODO: This should go in APICast.h
inline JSVirtualMachineRef toRef(Appcelerator::JSVirtualMachine* virtualMachine)
{
    return reinterpret_cast<JSVirtualMachineRef>(virtualMachine);
}

using namespace Appcelerator;

// From the API's perspective, a JSVirtualMachine remains alive iff
//     (a) it has been retained via JSVirtualMachineRetained
//     OR
//     (b) one of its JSContexts has been JSContextRetained

JSVirtualMachineRef JSVirtualMachineCreate()
{
    // TODO
    //    initializeThreading();
    return toRef(JSVirtualMachine::createJSVirtualMachine().leakRef());
}

JSVirtualMachineRef JSVirtualMachineCreateWithContextGroupRef(JSContextGroupRef group) {
    return toRef(JSVirtualMachine::createJSVirtualMachine(group).leakRef());
}

JSVirtualMachineRef JSVirtualMachineRetain(JSVirtualMachineRef virtualMachine) {
    toJS(virtualMachine)->ref();
    return virtualMachine;
}

void JJSVirtualMachineRelease(JSVirtualMachineRef virtualMachine) {
    JSVirtualMachine& vm = *toJS(virtualMachine);
    vm.deref();
}

