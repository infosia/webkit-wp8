/*
 * Copyright (C) 2013 Apple, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "ArrayIteratorPrototype.h"

#include "JSArrayIterator.h"
#include "JSCJSValueInlines.h"
#include "JSCellInlines.h"
#include "JSGlobalObject.h"
#include "ObjectConstructor.h"

namespace JSC {

const ClassInfo ArrayIteratorPrototype::s_info = { "Array Iterator", &Base::s_info, 0, 0, CREATE_METHOD_TABLE(ArrayIteratorPrototype) };

static EncodedJSValue JSC_HOST_CALL arrayIteratorPrototypeNext(ExecState*);

void ArrayIteratorPrototype::finishCreation(VM& vm, JSGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    vm.prototypeMap.addPrototype(this);

    JSC_NATIVE_FUNCTION(vm.propertyNames->next, arrayIteratorPrototypeNext, DontEnum, 0);
}

static EncodedJSValue createIteratorResult(CallFrame* callFrame, ArrayIterationKind kind, size_t index, JSValue result, bool done)
{
    JSGlobalObject* globalObject = callFrame->callee()->globalObject();
    JSObject* resultObject = constructEmptyObject(callFrame);
    resultObject->putDirect(callFrame->vm(), callFrame->propertyNames().done, jsBoolean(done));
    switch (kind & ~ArrayIterateSparseTag) {
    case ArrayIterateKey:
        resultObject->putDirect(callFrame->vm(), callFrame->propertyNames().value, done ? jsUndefined() : jsNumber(index));
        break;
    case ArrayIterateValue:
        resultObject->putDirect(callFrame->vm(), callFrame->propertyNames().value, done ? jsUndefined() : result);
        break;
    case ArrayIterateKeyValue: {
        if (!done) {
            MarkedArgumentBuffer args;
            args.append(jsNumber(index));
            args.append(result);
            JSArray* resultArray = constructArray(callFrame, 0, globalObject, args);
            resultObject->putDirect(callFrame->vm(), callFrame->propertyNames().value, resultArray);
        } else
            resultObject->putDirect(callFrame->vm(), callFrame->propertyNames().value, jsUndefined());

        break;
    }
    default:
        RELEASE_ASSERT_NOT_REACHED();
    }
    return JSValue::encode(resultObject);
}
    
EncodedJSValue JSC_HOST_CALL arrayIteratorPrototypeNext(CallFrame* callFrame)
{
    JSArrayIterator* iterator = jsDynamicCast<JSArrayIterator*>(callFrame->thisValue());
    if (!iterator)
        throwTypeError(callFrame, ASCIILiteral("Cannot call ArrayIterator.next() on a non-ArrayIterator object"));
    JSObject* iteratedObject = iterator->iteratedObject();
    size_t index = iterator->nextIndex();
    ArrayIterationKind kind = iterator->iterationKind();
    JSValue jsLength = JSValue(iteratedObject).get(callFrame, callFrame->propertyNames().length);
    if (callFrame->hadException())
        return JSValue::encode(jsNull());

    size_t length = jsLength.toUInt32(callFrame);
    if (callFrame->hadException())
        return JSValue::encode(jsNull());

    if (index >= length) {
        iterator->finish();
        return createIteratorResult(callFrame, kind, index, jsUndefined(), true);
    }
    if (JSValue result = iteratedObject->tryGetIndexQuickly(index)) {
        iterator->setNextIndex(index + 1);
        return createIteratorResult(callFrame, kind, index, result, false);
    }
    
    JSValue result = jsUndefined();
    PropertySlot slot(iteratedObject);
    if (kind > ArrayIterateSparseTag) {
        // We assume that the indexed property will be an own property so cache the getOwnProperty
        // method locally
        auto getOwnPropertySlotByIndex = iteratedObject->methodTable()->getOwnPropertySlotByIndex;
        while (index < length) {
            if (getOwnPropertySlotByIndex(iteratedObject, callFrame, index, slot)) {
                result = slot.getValue(callFrame, index);
                break;
            }
            if (iteratedObject->getPropertySlot(callFrame, index, slot)) {
                result = slot.getValue(callFrame, index);
                break;
            }
            index++;
        }
    } else if (iteratedObject->getPropertySlot(callFrame, index, slot))
        result = slot.getValue(callFrame, index);

    if (index == length)
        iterator->finish();
    else
        iterator->setNextIndex(index + 1);
    return createIteratorResult(callFrame, kind, index, jsUndefined(), index == length);
}

}
