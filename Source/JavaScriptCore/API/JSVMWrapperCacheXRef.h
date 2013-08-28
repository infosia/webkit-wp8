//
//  JSVMWrapperCacheXRef.h
//  JavaScriptCore
//
//  Created by Matt Langston on 8/27/13.
//
//

#ifndef JSVMWrapperCacheXRef_h
#define JSVMWrapperCacheXRef_h

#include <JavaScriptCore/JSBase.h>
//#include <JavaScriptCore/WebKitAvailability.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    /*! @typedef JSVMWrapperCacheXRef An instance of JSVMWrapperCacheX
     represents ... .
     */
    typedef const struct OpaqueJSVMWrapperCacheX* JSVMWrapperCacheXRef;
    
    /*!
     @function
     @abstract Create a JavaScript virtual machine wrapper cache.
     @discussion JSVMWrapperCacheXRef An instance of JSVMWrapperCacheX ... .
     @result The created JSVMWrapperCacheX.
     */
    JS_EXPORT JSVMWrapperCacheXRef JSVMWrapperCacheXCreate();
    
    /*!
     @function
     @abstract Retain a JSVMWrapperCacheX.
     @param wrapperCache The JSVMWrapperCacheX to retain.
     @result A JSVMWrapperCacheX that is the same as wrapperCache.
     */
    JS_EXPORT JSVMWrapperCacheXRef JSVMWrapperCacheXRetain(JSVMWrapperCacheXRef wrapperCache);
    
    /*!
     @function
     @abstract Release a JSVMWrapperCacheX.
     @param wrapperCache The JSVMWrapperCacheX to release.
     */
    JS_EXPORT void JSVMWrapperCacheXRelease(JSVMWrapperCacheXRef wrapperCache);
    
#ifdef __cplusplus
}
#endif


#endif /* JSVMWrapperCacheXRef_h */
