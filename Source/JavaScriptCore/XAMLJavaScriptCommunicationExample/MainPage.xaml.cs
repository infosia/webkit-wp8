using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;
using PhoneApp1.Resources;
using JavaScriptCoreComponent;

namespace PhoneApp1
{
    public sealed class TextBlockImpl : ITextBlock
    {
        TextBlock textBlock;

        public String Text
        {
            get { return this.textBlock.Text; }
            set { this.textBlock.Text = value; }
        }

        public TextBlockImpl(TextBlock textBlock)
        {
            this.textBlock = textBlock;
        }
    }

    public partial class MainPage : PhoneApplicationPage
    {
        private JavaScriptCoreExecutor executor = new JavaScriptCoreExecutor();

        // Constructor
        public MainPage()
        {
            InitializeComponent();
            executor.GiveTextBlock(new TextBlockImpl(InjectedBox));
        }

        private void ExecuteClickHandler(object sender, RoutedEventArgs e)
        {
            ScriptOutput.Text = executor.ExecuteScript(ScriptInput.Text);
        }
    }
}