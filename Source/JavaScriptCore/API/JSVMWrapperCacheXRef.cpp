//
//  JSVMWrapperCacheXRef.cpp
//  JavaScriptCore
//
//  Created by Matt Langston on 8/27/13.
//
//

#include "config.h"
#include "JSVMWrapperCacheXRef.h"
#include "JSContextRef.h"
//#include <wtf/Forward.h>
#include <wtf/PassRefPtr.h>
#include <wtf/ThreadSafeRefCounted.h>

namespace Appcelerator {
    
    class JSVMWrapperCache : public ThreadSafeRefCounted<JSVMWrapperCache> {
    public:
        
        static PassRefPtr<JSVMWrapperCache> createJSVMWrapperCache();
        JS_EXPORT_PRIVATE ~JSVMWrapperCache();
        
    private:
        JSVMWrapperCache();
    };
    
} // namespace Appcelerator


namespace Appcelerator {
    
    JSVMWrapperCache::JSVMWrapperCache() {
    }
    
    JSVMWrapperCache::~JSVMWrapperCache() {
        //JSContextGroupRelease(m_group);
    }
    
    PassRefPtr<JSVMWrapperCache> JSVMWrapperCache::createJSVMWrapperCache() {
        return adoptRef(new JSVMWrapperCache());
    }
    
} // namespace Appcelerator

// TODO: This should go in APICast.h
inline Appcelerator::JSVMWrapperCache* toJS(JSVMWrapperCacheXRef wrapperCache)
{
    return reinterpret_cast<Appcelerator::JSVMWrapperCache*>(const_cast<OpaqueJSVMWrapperCacheX*>(wrapperCache));
}

// TODO: This should go in APICast.h
inline JSVMWrapperCacheXRef toRef(Appcelerator::JSVMWrapperCache* wrapperCache)
{
    return reinterpret_cast<JSVMWrapperCacheXRef>(wrapperCache);
}

//using namespace Appcelerator;

JSVMWrapperCacheXRef JSVMWrapperCacheXCreate()
{
    // TODO
    //    initializeThreading();
    return toRef(Appcelerator::JSVMWrapperCache::createJSVMWrapperCache().leakRef());
}

JSVMWrapperCacheXRef JSVMWrapperCacheXRetain(JSVMWrapperCacheXRef wrapperCache) {
    toJS(wrapperCache)->ref();
    return wrapperCache;
}

void JSVMWrapperCacheXRelease(JSVMWrapperCacheXRef jsWrapperCache) {
    Appcelerator::JSVMWrapperCache *wrapperCache = toJS(jsWrapperCache);
    if (wrapperCache) {
        wrapperCache->deref();
    }
}

