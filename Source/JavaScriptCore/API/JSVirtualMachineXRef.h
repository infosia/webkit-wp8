//
//  JSVirtualMachineXRef.h
//  JavaScriptCore
//
//  Created by Matt Langston on 8/22/13.
//
//

#ifndef JSVirtualMachineXRef_h
#define JSVirtualMachineXRef_h

#include <JavaScriptCore/JSBase.h>
#include <JavaScriptCore/JSManagedValueXRef.h>
//#include <JavaScriptCore/WebKitAvailability.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    /*! @typedef JSVirtualMachineXRef An instance of JSVirtualMachineX
     represents a single JavaScript "object space" or set of execution
     resources. Thread safety is supported by locking the virtual machine,
     with concurrent JavaScript execution supported by allocating separate
     instances of JSVirtualMachineX.
     */
    typedef const struct OpaqueJSVirtualMachineX* JSVirtualMachineXRef;
    
    /*!
     @function
     @abstract Create a JavaScript virtual machine.
     @discussion JSVirtualMachineXRef An instance of JSVirtualMachineX
     represents a single JavaScript "object space" or set of execution
     resources. Thread safety is supported by locking the virtual machine,
     with concurrent JavaScript execution supported by allocating separate
     instances of JSVirtualMachineX.
     @result The created JSVirtualMachineX.
     */
    JS_EXPORT JSVirtualMachineXRef JSVirtualMachineXCreate();
    
    /*!
     @function
     @abstract Retain a JSVirtualMachineX.
     @param virtualMachine The JSVirtualMachineX to retain.
     @result A JSVirtualMachineX that is the same as virtualMachine.
     */
    JS_EXPORT JSVirtualMachineXRef JSVirtualMachineXRetain(JSVirtualMachineXRef virtualMachine);
    
    /*!
     @function
     @abstract Release a JSVirtualMachineX.
     @param virtualMachine The JSVirtualMachineX to release.
     */
    JS_EXPORT void JSVirtualMachineXRelease(JSVirtualMachineXRef virtualMachine);
    
    /*!
     @function
     @abstract Register a JSManagedValueX.
     @discussion addManagedReference:withOwner and
     removeManagedReference:withOwner allow clients of JSVirtualMachineX to make
     the JavaScript runtime aware of arbitrary external object graphs. The
     runtime can then use this information to retain any JavaScript values that
     are referenced from somewhere in said object graph.
     
     For correct behavior clients must make their external object graphs
     reachable from within the JavaScript runtime. If an external object is
     reachable from within the JavaScript runtime, all managed references
     transitively reachable from it as recorded with
     addManagedReference:withOwner: will be scanned by the garbage collector.
     @param managedValue The JSManagedValueX to register.
     */
    //    JS_EXPORT void JSVirtualMachineXAddManagedReferenceWithOwner(JSManagedValueXRef managedValue, void *owner);
    
#ifdef __cplusplus
}
#endif


#endif /* JSVirtualMachineXRef_h */
