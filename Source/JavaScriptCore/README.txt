 -*- mode: Org -*-

Author: Matt Langston
Date: 2013.08.06

This directory contains an Xcode5 workspace named
JavaScriptCore.xcworkspace that includes:

1. Appcelerator's JavaScriptCore OO API project.

   1. This include Appcelerator's unit tests for JavaScriptCore
      "Objective-C API".

2. The JavaScriptCore project.

3. The WTF project (dependency of the JavaScriptCore project).


Follow these steps to open the JavaScriptCore.xcworkspace

1. Make sure you have Xcode5-DP5 installed from
   https://developer.apple.com.

2. If not already done so, apply the patch JavaScriptCore_API.patch to
   the JavaScriptCore sources. In the following steps replace
   @WEBKIT_SRC_DIR@ with the directory path to your WebKit git
   repository.

   1. cd @WEBKIT_SRC_DIR@/Source/JavaScriptCore/

   2. patch -p3 < JavaScriptCore_API.patch
