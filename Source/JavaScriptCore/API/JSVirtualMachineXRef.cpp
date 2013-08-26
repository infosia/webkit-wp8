//
//  JSVirtualMachineXRef.cpp
//  JavaScriptCore
//
//  Created by Matt Langston on 8/22/13.
//
//

#include "config.h"
#include "JSVirtualMachineXRef.h"
#include "JSVirtualMachineXPrivateRef.h"
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
    
    PassRefPtr<JSVirtualMachine> JSVirtualMachine::createJSVirtualMachine() {
        return adoptRef(new JSVirtualMachine());
    }
    
    PassRefPtr<JSVirtualMachine> JSVirtualMachine::createJSVirtualMachine(JSContextGroupRef group) {
        return adoptRef(new JSVirtualMachine(group));
    }
    
} // namespace Appcelerator

// TODO: This should go in APICast.h
inline Appcelerator::JSVirtualMachine* toJS(JSVirtualMachineXRef virtualMachine)
{
    return reinterpret_cast<Appcelerator::JSVirtualMachine*>(const_cast<OpaqueJSVirtualMachineX*>(virtualMachine));
}

// TODO: This should go in APICast.h
inline JSVirtualMachineXRef toRef(Appcelerator::JSVirtualMachine* virtualMachine)
{
    return reinterpret_cast<JSVirtualMachineXRef>(virtualMachine);
}

//using namespace Appcelerator;

// From the API's perspective, a JSVirtualMachine remains alive iff
//     (a) it has been retained via JSVirtualMachineRetained
//     OR
//     (b) one of its JSContexts has been JSContextRetained

JSVirtualMachineXRef JSVirtualMachineXCreate()
{
    // TODO
    //    initializeThreading();
    return toRef(Appcelerator::JSVirtualMachine::createJSVirtualMachine().leakRef());
}

JSVirtualMachineXRef JSVirtualMachineXCreateWithContextGroupRef(JSContextGroupRef group) {
    return toRef(Appcelerator::JSVirtualMachine::createJSVirtualMachine(group).leakRef());
}

JSVirtualMachineXRef JSVirtualMachineXRetain(JSVirtualMachineXRef virtualMachine) {
    toJS(virtualMachine)->ref();
    return virtualMachine;
}

void JSVirtualMachineXRelease(JSVirtualMachineXRef jsVirtualMachine) {
    Appcelerator::JSVirtualMachine *virtualMachine = toJS(jsVirtualMachine);
    if (virtualMachine) {
        virtualMachine->deref();
    }
}

