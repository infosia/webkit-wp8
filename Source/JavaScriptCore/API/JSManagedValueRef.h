//
//  JSManagedValueRef.h
//  JavaScriptCore
//
//  Created by Matt Langston on 8/22/13.
//
//

#ifndef JSManagedValueRef_h
#define JSManagedValueRef_h

#include <JavaScriptCore/JSBase.h>
//#include <JavaScriptCore/WebKitAvailability.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    /*! @typedef JSManagedValueRef An instance of JSManagedValue represents a
     "conditionally retained" JSValue. "Conditionally retained" means that as
     long as either the JSManagedValue JavaScript value is reachable through the
     JavaScript object graph or the JSManagedValue external object is reachable
     through the external object graph as reported to the JSVirtualMachine using
     JJSVirtualMachineaAdManagedReferenceWithOwner, then the corresponding JavaScript
     value will not be garbage collected. However, if neither of these
     conditions are true, then the corresponding JSValue will be released and
     set to nil.
     
     The primary use case for JSManagedValue is for safely referencing JSValues
     from the native heap. It is incorrect to store a JSValue into an external
     heap object, as this can very easily create a reference cycle, keeping the
     entire JSContext alive.
     */
    typedef const struct OpaqueJSManagedValue* JSManagedValueRef;
    
#ifdef __cplusplus
}
#endif

#endif // JSManagedValueRef_h

