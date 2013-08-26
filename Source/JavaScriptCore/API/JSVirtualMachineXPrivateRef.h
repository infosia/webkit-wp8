//
//  JSVirtualMachineXRefPrivate.h
//  JavaScriptCore
//
//  Created by Matt Langston on 8/22/13.
//
//

#ifndef JSVirtualMachineXPrivateRef_h
#define JSVirtualMachineXPrivateRef_h

#include "JSVirtualMachineXRef.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    JS_EXPORT JSVirtualMachineXRef JSVirtualMachineXCreateWithContextGroupRef(JSContextGroupRef group);
    
#ifdef __cplusplus
}
#endif

#endif /* JSVirtualMachineXPrivateRef_h */
