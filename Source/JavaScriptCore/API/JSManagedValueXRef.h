//
//  JSManagedValueXRef.h
//  JavaScriptCore
//
//  Created by Matt Langston on 8/22/13.
//
//

#ifndef JSManagedValueXRef_h
#define JSManagedValueXRef_h

#include <JavaScriptCore/JSBase.h>
//#include <JavaScriptCore/WebKitAvailability.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    /*! @typedef JSManagedValueXRefX An instance of a JSManagedValueXRefX
     by itself represents a weak reference to a JSValueRefX, whereas a
     JSValueRefX represents a strong reference to a JavaScript value.
     
     meaning that the
     JavaScript garbage collector will reclaim this JSValueRefX when there are
     no references to it from JavaScript.
     
     However, a JSManagedValueXRefX can be transformed from a weak reference to a
     garbage collected referene if it is registered with a
     JSVirtualMachineRefX.
     
     Without a this new type of reference, What this means is that The idea is that a
     
     "conditionally retained" JSValue.
     
     
     "Conditionally retained" means that as
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
    typedef const struct OpaqueJSManagedValueX* JSManagedValueXRef;
    
#ifdef __cplusplus
}
#endif

#endif /* JSManagedValueXRef_h */

