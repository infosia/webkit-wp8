#include "pch.h"
#include "JSTextBlock.h"
#include "JavaScriptCoreComponent.h"
#include <JavaScriptCore/JSObjectRef.h>
#include <JavaScriptCore/JSStringRef.h>
#include <string>

using namespace JavaScriptCoreComponent;
using namespace Platform;

JavaScriptCoreExecutor::JavaScriptCoreExecutor()
    : m_context(JSGlobalContextCreate(NULL))
{
}

JavaScriptCoreExecutor::~JavaScriptCoreExecutor()
{
    JSGlobalContextRelease(m_context);
}

String^ JavaScriptCoreExecutor::ExecuteScript(String^ script)
{
    JSStringRef string = JSStringCreateWithCharacters(script->Data(), script->Length());
    JSValueRef value = JSEvaluateScript(m_context, string, NULL, NULL, 0, NULL);
    
    JSStringRef resultString = JSValueToStringCopy(m_context, value, NULL);
	std::wstring resultCString(JSStringGetCharactersPtr(resultString), JSStringGetLength(resultString));

    JSStringRelease(string);
    JSStringRelease(resultString);

	return ref new String(resultCString.c_str());
}

void JavaScriptCoreExecutor::GiveTextBlock(ITextBlock^ textBlock)
{
    JSObjectRef newObject = JSTextBlock::Create(m_context, textBlock);

    const wchar_t* propertyName = L"textBlock";
    JSStringRef propertyNameString = JSStringCreateWithCharacters(propertyName, wcslen(propertyName));

    JSObjectSetProperty(m_context, JSContextGetGlobalObject(m_context), propertyNameString, newObject, kJSPropertyAttributeNone, NULL);
}
