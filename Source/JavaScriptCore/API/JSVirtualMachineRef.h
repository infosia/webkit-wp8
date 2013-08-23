//
//  JSVirtualMachineRef.h
//  JavaScriptCore
//
//  Created by Matt Langston on 8/22/13.
//
//

#ifndef JSVirtualMachineRef_h
#define JSVirtualMachineRef_h

#include <JavaScriptCore/JSBase.h>
#include <JavaScriptCore/JSManagedValueRef.h>
//#include <JavaScriptCore/WebKitAvailability.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    /*! @typedef JSVirtualMachineRef An instance of JSVirtualMachine
     represents a single JavaScript "object space" or set of execution
     resources. Thread safety is supported by locking the virtual machine,
     with concurrent JavaScript execution supported by allocating separate
     instances of JSVirtualMachine.
     */
    typedef const struct OpaqueJSVirtualMachine* JSVirtualMachineRef;
    
    /*!
     @function
     @abstract Creates a JavaScript virtual machine.
     @discussion SVirtualMachineRef An instance of JSVirtualMachine
     represents a single JavaScript "object space" or set of execution
     resources. Thread safety is supported by locking the virtual machine,
     with concurrent JavaScript execution supported by allocating separate
     instances of JSVirtualMachine.
     @result The created JSVirtualMachine.
     */
    JS_EXPORT JSVirtualMachineRef JSVirtualMachineCreate();
    
    /*!
     @function
     @abstract Retains a JSVirtualMachine.
     @param virtualMachine The JSVirtualMachine to retain.
     @result A JSVirtualMachine that is the same as virtualMachine.
     */
    JS_EXPORT JSVirtualMachineRef JSVirtualMachineRetain(JSVirtualMachineRef virtualMachine);
    
    /*!
     @function
     @abstract Releases a JSVirtualMachine.
     @param virtualMachine The JSVirtualMachine to release.
     */
    JS_EXPORT void JJSVirtualMachineRelease(JSVirtualMachineRef virtualMachine);
    
    /*!
     @function
     @abstract Releases a JSVirtualMachine.
     @discussion addManagedReference:withOwner and
     removeManagedReference:withOwner allow clients of JSVirtualMachine to make
     the JavaScript runtime aware of arbitrary external object graphs. The
     runtime can then use this information to retain any JavaScript values that
     are referenced from somewhere in said object graph.
     
     For correct behavior clients must make their external object graphs
     reachable from within the JavaScript runtime. If an external object is
     reachable from within the JavaScript runtime, all managed references
     transitively reachable from it as recorded with
     addManagedReference:withOwner: will be scanned by the garbage collector.
     @param managedValue The JSManagedValue to register.
     */
    JS_EXPORT void JJSVirtualMachineaAdManagedReferenceWithOwner(JSManagedValueRef managedValue, void *owner);

#ifdef __cplusplus
}
#endif


#endif // JSVirtualMachineRef_h
