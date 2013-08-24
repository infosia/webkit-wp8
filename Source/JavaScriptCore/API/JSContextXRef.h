//
//  JSContextXRef.h
//  JavaScriptCore
//
//  Created by Matt Langston on 8/23/13.
//
//

#ifndef JSContextXRef_h
#define JSContextXRef_h

#include <JavaScriptCore/JSBase.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    /*! @typedef JSContextXRef An instance of JSContextXRef represents a
     JavaScript execution environment. All JavaScript execution takes place
     within a context. JSContextXRef is also used to manage the life-cycle of
     objects within the JavaScript virtual machine. Every instance of
     JSValueXRef is associated with a JSContextXRef via a strong reference. The
     JSValueXRef will keep the JSContextXRef it references alive so long as the
     JSValueXRef remains alive. When all of the JSValueXRefs that reference a
     particular JSContextXRef have been deallocated the JSContextXRef will be
     deallocated unless it has been previously retained.
     */
    typedef struct OpaqueJSContextX* JSContextXRef;
    
    /*!
     @function
     @abstract Create a JSContextXRef.
     @result The created JSVirtualMachine.
     */
    JS_EXPORT JSContextXRef JSContextXCreate();
    
    /*!
     @function
     @abstract Retain a JSContextXRef.
     @param jsContext The JSContextXRef to retain.
     @result A JSContextXRef that is the same as jsContext.
     */
    JS_EXPORT JSContextXRef JSContextXRetain(JSContextXRef jsContext);
    
    /*!
     @function
     @abstract Release a JSContextXRef.
     @param jsContext The JSContextXRef to release.
     */
    JS_EXPORT void JSContextXRelease(JSContextXRef jsContext);
    
#ifdef __cplusplus
}
#endif

#endif /* JSContextXRef_h */
