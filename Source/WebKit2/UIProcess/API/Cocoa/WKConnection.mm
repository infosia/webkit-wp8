/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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
#import "WKConnectionInternal.h"

#if WK_API_ENABLED

#import "ObjCObjectGraph.h"
#import "WKConnectionRef.h"
#import "WKData.h"
#import "WKRetainPtr.h"
#import "WKStringCF.h"
#import "WKRemoteObjectRegistryInternal.h"
#import <wtf/RetainPtr.h>

using namespace WebKit;

@implementation WKConnection {
    // Underlying connection object.
    WKRetainPtr<WKConnectionRef> _connectionRef;

    RetainPtr<WKRemoteObjectRegistry> _remoteObjectRegistry;
}

- (void)dealloc
{
    WKConnectionSetConnectionClient(_connectionRef.get(), 0);

    [super dealloc];
}

- (void)sendMessageWithName:(NSString *)messageName body:(id)messageBody
{
    WKRetainPtr<WKStringRef> wkMessageName = adoptWK(WKStringCreateWithCFString((CFStringRef)messageName));
    RefPtr<ObjCObjectGraph> wkMessageBody = ObjCObjectGraph::create(messageBody);

    WKConnectionPostMessage(_connectionRef.get(), wkMessageName.get(), (WKTypeRef)wkMessageBody.get());
}

- (WKRemoteObjectRegistry *)remoteObjectRegistry
{
    if (!_remoteObjectRegistry)
        _remoteObjectRegistry = adoptNS([[WKRemoteObjectRegistry alloc] _initWithConnectionRef:_connectionRef.get()]);

    return _remoteObjectRegistry.get();
}

static void didReceiveMessage(WKConnectionRef, WKStringRef messageName, WKTypeRef messageBody, const void* clientInfo)
{
    WKConnection *connection = (WKConnection *)clientInfo;

    if ([connection->_remoteObjectRegistry _handleMessageWithName:messageName body:messageBody])
        return;

    if ([connection.delegate respondsToSelector:@selector(connection:didReceiveMessageWithName:body:)]) {
        RetainPtr<CFStringRef> nsMessageName = adoptCF(WKStringCopyCFString(kCFAllocatorDefault, messageName));
        RetainPtr<id> nsMessageBody = ((ObjCObjectGraph*)messageBody)->rootObject();

        [connection.delegate connection:connection didReceiveMessageWithName:(NSString *)nsMessageName.get() body:nsMessageBody.get()];
    }
}

static void didClose(WKConnectionRef, const void* clientInfo)
{
    WKConnection *connection = (WKConnection *)clientInfo;
    if ([connection.delegate respondsToSelector:@selector(connectionDidClose:)]) {
        [connection.delegate connectionDidClose:connection];
    }
}

static void setUpClient(WKConnection *connection, WKConnectionRef connectionRef)
{
    WKConnectionClientV0 client;
    memset(&client, 0, sizeof(client));

    client.base.version = 0;
    client.base.clientInfo = connection;
    client.didReceiveMessage = didReceiveMessage;
    client.didClose = didClose;

    WKConnectionSetConnectionClient(connectionRef, &client.base);
}

- (id)_initWithConnectionRef:(WKConnectionRef)connectionRef
{
    self = [super init];
    if (!self)
        return nil;

    _connectionRef = connectionRef;

    setUpClient(self, _connectionRef.get());

    return self;
}

@end

#endif // WK_API_ENABLED
