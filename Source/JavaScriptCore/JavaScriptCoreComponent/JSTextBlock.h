#include "JavaScriptCoreComponent.h"
#include <JavaScriptCore/JSContextRef.h>
#include <JavaScriptCore/JSStringRef.h>

namespace JavaScriptCoreComponent
{
    class JSTextBlock
    {
    public:
        static JSObjectRef JSTextBlock::Create(JSContextRef, ITextBlock^);

    private:
        static JSClassRef getClass();
        static JSValueRef TextGetter(JSContextRef, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception);
        static bool TextSetter(JSContextRef, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception);
        static void finalize(JSObjectRef object);
        
        JSTextBlock(ITextBlock^);
        ITextBlock^ m_textBlock;
    };

}