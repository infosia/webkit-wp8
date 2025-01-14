/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "config.h"
#import "UserAgent.h"

#import <WebCoreSystemInterface.h>
#import <wtf/NeverDestroyed.h>
#import <wtf/RetainPtr.h>

namespace WebCore {

static NSString *platformSystemRootDirectory()
{
#if PLATFORM(IOS_SIMULATOR)
    const char *simulatorRoot = getenv("IPHONE_SIMULATOR_ROOT");
    return [NSString stringWithUTF8String:(simulatorRoot ? simulatorRoot : "/")];
#else
    return @"/";
#endif
}

static NSString *osMarketingVersion()
{
    RetainPtr<NSDictionary> systemInfo = adoptNS([[NSDictionary alloc] initWithContentsOfFile:[platformSystemRootDirectory() stringByAppendingPathComponent:@"System/Library/CoreServices/SystemVersion.plist"]]);
    NSString *productVersion = [systemInfo objectForKey:@"ProductVersion"];
    return !productVersion ? @"" : [productVersion stringByReplacingOccurrencesOfString:@"." withString:@"_"];
}

String standardUserAgentWithApplicationName(const String& applicationName, const String& webkitVersionString)
{
    if (CFStringRef overrideUserAgent = wkGetUserAgent())
        return overrideUserAgent;

    // Check to see if there is a user agent override for all WebKit clients.
    CFPropertyListRef override = CFPreferencesCopyAppValue(CFSTR("UserAgent"), CFSTR("com.apple.WebFoundation"));
    if (override) {
        if (CFGetTypeID(override) == CFStringGetTypeID())
            return static_cast<NSString *>(CFBridgingRelease(override));
        CFRelease(override);
    }

    NSString *webKitVersion = webkitVersionString;
    CFStringRef deviceName = wkGetDeviceName();
    CFStringRef osNameForUserAgent = wkGetOSNameForUserAgent();
    NSString *osMarketingVersionString = osMarketingVersion();
    if (applicationName.isEmpty())
        return [NSString stringWithFormat:@"Mozilla/5.0 (%@; CPU %@ %@ like Mac OS X) AppleWebKit/%@ (KHTML, like Gecko)", deviceName, osNameForUserAgent, osMarketingVersionString, webKitVersion];
    return [NSString stringWithFormat:@"Mozilla/5.0 (%@; CPU %@ %@ like Mac OS X) AppleWebKit/%@ (KHTML, like Gecko) %@", deviceName, osNameForUserAgent, osMarketingVersionString, webKitVersion, static_cast<NSString *>(applicationName)];
}

} // namespace WebCore.
