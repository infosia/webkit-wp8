//
//  JSContextXRef.cpp
//  JavaScriptCore
//
//  Created by Matt Langston on 8/23/13.
//
//

#include "config.h"
#include "JSContextXRef.h"
//#include <wtf/Forward.h>
#include <wtf/PassRefPtr.h>
#include <wtf/ThreadSafeRefCounted.h>

namespace Appcelerator {
    
    class JSContext : public ThreadSafeRefCounted<JSContext> {
    public:
        
        static PassRefPtr<JSContext> createJSContext();
        JS_EXPORT_PRIVATE ~JSContext();
        
    private:
        JSContext();
    };
    
} // namespace Appcelerator


namespace Appcelerator {
    
    JSContext::JSContext() {
    }
    
    JSContext::~JSContext() {
    }
    
    PassRefPtr<JSContext> JSContext::createJSContext() {
        return adoptRef(new JSContext());
    }
    
} // namespace Appcelerator

// TODO: This should go in APICast.h
inline Appcelerator::JSContext* toJS(JSContextXRef jsContext) {
    return reinterpret_cast<Appcelerator::JSContext*>(const_cast<OpaqueJSContextX*>(jsContext));
}

// TODO: This should go in APICast.h
inline JSContextXRef toRef(Appcelerator::JSContext* jsContext) {
    return reinterpret_cast<JSContextXRef>(jsContext);
}

//using namespace Appcelerator;

// From the API's perspective, a JSVirtualMachine remains alive iff
//     (a) it has been retained via JSVirtualMachineRetained
//     OR
//     (b) one of its JSContexts has been JSContextRetained

JSContextXRef JSContextXCreate() {
    return toRef(Appcelerator::JSContext::createJSContext().leakRef());
}

JSContextXRef JSContextXRetain(JSContextXRef jsContext) {
    toJS(jsContext)->ref();
    return jsContext;
}

void JSContextXRelease(JSContextXRef jsContext) {
    Appcelerator::JSContext& context = *toJS(jsContext);
    context.deref();
}

