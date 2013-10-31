/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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
#import "WKRemoteObjectRegistry.h"
#import "WKRemoteObjectRegistryPrivate.h"

#import "Connection.h"
#import "WKConnectionRef.h"
#import "WebConnection.h"
#import "WKSharedAPICast.h"

#if WK_API_ENABLED

using namespace WebKit;

@implementation WKRemoteObjectRegistry {
    RefPtr<WebConnection> _connection;
}

- (void)registerExportedObject:(id)object interface:(WKRemoteObjectInterface *)interface
{
    // FIXME: Implement.
}

- (void)unregisterExportedObject:(id)object interface:(WKRemoteObjectInterface *)interface
{
    // FIXME: Implement.
}

- (id)remoteObjectProxyWithInterface:(WKRemoteObjectInterface *)interface
{
    // FIXME: Implement.
    return nil;
}

@end

@implementation WKRemoteObjectRegistry (WKPrivate)

- (id)_initWithConnectionRef:(WKConnectionRef)connectionRef
{
    if (!(self = [super init]))
        return nil;

    _connection = toImpl(connectionRef);

    return self;
}

- (BOOL)_handleMessageWithName:(WKStringRef)name body:(WKTypeRef)body
{
    // FIXME: Implement.
    return NO;
}

@end

#endif // WK_API_ENABLED
