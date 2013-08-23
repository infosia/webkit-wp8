//
//  JSVirtualMachineRefPrivate.h
//  JavaScriptCore
//
//  Created by Matt Langston on 8/22/13.
//
//

#ifndef JSVirtualMachineRefPrivate_h
#define JSVirtualMachineRefPrivate_h

#include "JSVirtualMachineRef.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    JS_EXPORT JSVirtualMachineRef JSVirtualMachineCreateWithContextGroupRef(JSContextGroupRef group);
    
#ifdef __cplusplus
}
#endif

#endif // JSVirtualMachineRefPrivate_h
