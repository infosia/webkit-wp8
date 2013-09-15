#include "pch.h"
#include "JSTextBlock.h"
#include <JavaScriptCore/JSObjectRef.h>
#include <assert.h>
#include <string>

using namespace JavaScriptCoreComponent;
using namespace Platform;

JSTextBlock::JSTextBlock(ITextBlock^ textBlock)
    : m_textBlock(textBlock)
{
}

JSObjectRef JSTextBlock::Create(JSContextRef context, ITextBlock^ textBlock)
{
    return JSObjectMake(context, JSTextBlock::getClass(), new JSTextBlock(textBlock));
}

JSClassRef JSTextBlock::getClass()
{
    static JSClassRef jsClass = nullptr;
    if (jsClass)
        return jsClass;

    static JSStaticValue staticValues[] = {
        { "Text", JSTextBlock::TextGetter, JSTextBlock::TextSetter, kJSPropertyAttributeDontDelete },
        { 0, 0, 0, 0 }
    };

    JSClassDefinition definition = kJSClassDefinitionEmpty;
    definition.staticValues = staticValues;
    definition.finalize = JSTextBlock::finalize;
    jsClass = JSClassCreate(&definition);
    return jsClass;
}

void JSTextBlock::finalize(JSObjectRef object)
{
    JSTextBlock* block = static_cast<JSTextBlock*>(JSObjectGetPrivate(object));
    assert(block);
    delete block;
}

JSValueRef JSTextBlock::TextGetter(JSContextRef context, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
{
    JSTextBlock* block = static_cast<JSTextBlock*>(JSObjectGetPrivate(object));
    JSStringRef string = JSStringCreateWithCharacters(block->m_textBlock->Text->Data(), block->m_textBlock->Text->Length());
    JSValueRef returnValue = JSValueMakeString(context, string);
    JSStringRelease(string);
    return returnValue;
}

bool JSTextBlock::TextSetter(JSContextRef context, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
{
    JSTextBlock* block = static_cast<JSTextBlock*>(JSObjectGetPrivate(object));
    JSStringRef newText = JSValueToStringCopy(context, value, exception);
    if (!newText)
        return true;

    std::wstring newTextCString(JSStringGetCharactersPtr(newText), JSStringGetLength(newText));
    block->m_textBlock->Text = ref new String(newTextCString.c_str());
    JSStringRelease(newText);
    return true;
}