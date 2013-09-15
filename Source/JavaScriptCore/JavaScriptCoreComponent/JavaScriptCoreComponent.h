#pragma once

#include <JavaScriptCore/JSContextRef.h>

namespace JavaScriptCoreComponent
{
    
	public interface class ITextBlock
    {
	public:
        property Platform::String^ Text;
	};

    public ref class JavaScriptCoreExecutor sealed
    {
    public:
        JavaScriptCoreExecutor();
        virtual ~JavaScriptCoreExecutor();

        Platform::String^ ExecuteScript(Platform::String^ script);
        void GiveTextBlock(ITextBlock^ textField);

    private:
        JSGlobalContextRef m_context;
    };
}