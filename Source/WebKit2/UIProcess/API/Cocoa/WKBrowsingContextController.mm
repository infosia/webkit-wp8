/*
 * Copyright (C) 2011, 2012, 2013 Apple Inc. All rights reserved.
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
#import "WKBrowsingContextControllerInternal.h"

#if WK_API_ENABLED

#import "WeakObjCPtr.h"
#import "ObjCObjectGraph.h"
#import "WKBackForwardListInternal.h"
#import "WKBackForwardListItemInternal.h"
#import "WKErrorRecoveryAttempting.h"
#import "WKFrame.h"
#import "WKFramePolicyListener.h"
#import "WKNSArray.h"
#import "WKNSError.h"
#import "WKNSURLAuthenticationChallenge.h"
#import "WKNSURLExtras.h"
#import "WKNSURLProtectionSpace.h"
#import "WKRetainPtr.h"
#import "WKURLRequestNS.h"
#import "WKURLResponseNS.h"
#import "WebContext.h"
#import "WebData.h"
#import "WebPageProxy.h"

#import "WKBrowsingContextGroupInternal.h"
#import "WKBrowsingContextHandleInternal.h"
#import "WKBrowsingContextLoadDelegatePrivate.h"
#import "WKBrowsingContextPolicyDelegate.h"
#import "WKProcessGroupInternal.h"

using namespace WebCore;
using namespace WebKit;

class PageLoadStateObserver : public PageLoadState::Observer {
public:
    PageLoadStateObserver(WKBrowsingContextController *controller)
        : m_controller(controller)
    {
    }

private:
    virtual void willChangeIsLoading() OVERRIDE
    {
        [m_controller willChangeValueForKey:@"loading"];
    }

    virtual void didChangeIsLoading() OVERRIDE
    {
        [m_controller didChangeValueForKey:@"loading"];
    }

    virtual void willChangeTitle() OVERRIDE
    {
        [m_controller willChangeValueForKey:@"title"];
    }

    virtual void didChangeTitle() OVERRIDE
    {
        [m_controller didChangeValueForKey:@"title"];
    }

    virtual void willChangeEstimatedProgress() OVERRIDE
    {
        [m_controller willChangeValueForKey:@"estimatedProgress"];
    }

    virtual void didChangeEstimatedProgress() OVERRIDE
    {
        [m_controller didChangeValueForKey:@"estimatedProgress"];
    }

    WKBrowsingContextController *m_controller;
};

NSString * const WKActionIsMainFrameKey = @"WKActionIsMainFrameKey";
NSString * const WKActionNavigationTypeKey = @"WKActionNavigationTypeKey";
NSString * const WKActionMouseButtonKey = @"WKActionMouseButtonKey";
NSString * const WKActionModifierFlagsKey = @"WKActionModifierFlagsKey";
NSString * const WKActionURLRequestKey = @"WKActionURLRequestKey";
NSString * const WKActionURLResponseKey = @"WKActionURLResponseKey";
NSString * const WKActionFrameNameKey = @"WKActionFrameNameKey";
NSString * const WKActionOriginatingFrameURLKey = @"WKActionOriginatingFrameURLKey";
NSString * const WKActionCanShowMIMETypeKey = @"WKActionCanShowMIMETypeKey";

static NSString * const frameErrorKey = @"WKBrowsingContextFrameErrorKey";

@interface WKBrowsingContextController () <WKErrorRecoveryAttempting>
@end

@implementation WKBrowsingContextController {
    API::ObjectStorage<WebPageProxy> _page;
    std::unique_ptr<PageLoadStateObserver> _pageLoadStateObserver;

    WeakObjCPtr<id <WKBrowsingContextLoadDelegate>> _loadDelegate;
    WeakObjCPtr<id <WKBrowsingContextPolicyDelegate>> _policyDelegate;
}

- (void)dealloc
{
    _page->pageLoadState().removeObserver(*_pageLoadStateObserver);
    _page->~WebPageProxy();

    [super dealloc];
}

- (void)_finishInitialization
{
    _pageLoadStateObserver = std::make_unique<PageLoadStateObserver>(self);
    _page->pageLoadState().addObserver(*_pageLoadStateObserver);
}

- (WKProcessGroup *)processGroup
{
    return wrapper(_page->process().context());
}

- (WKBrowsingContextGroup *)browsingContextGroup
{
    return wrapper(_page->pageGroup());
}

#pragma mark Loading

+ (void)registerSchemeForCustomProtocol:(NSString *)scheme
{
    if (!scheme)
        return;

    NSString *lowercaseScheme = [scheme lowercaseString];
    [[WKBrowsingContextController customSchemes] addObject:lowercaseScheme];
    [[NSNotificationCenter defaultCenter] postNotificationName:SchemeForCustomProtocolRegisteredNotificationName object:lowercaseScheme];
}

+ (void)unregisterSchemeForCustomProtocol:(NSString *)scheme
{
    if (!scheme)
        return;

    NSString *lowercaseScheme = [scheme lowercaseString];
    [[WKBrowsingContextController customSchemes] removeObject:lowercaseScheme];
    [[NSNotificationCenter defaultCenter] postNotificationName:SchemeForCustomProtocolUnregisteredNotificationName object:lowercaseScheme];
}

- (void)loadRequest:(NSURLRequest *)request
{
    [self loadRequest:request userData:nil];
}

- (void)loadRequest:(NSURLRequest *)request userData:(id)userData
{
    RefPtr<WebURLRequest> wkURLRequest = WebURLRequest::create(request);

    RefPtr<ObjCObjectGraph> wkUserData;
    if (userData)
        wkUserData = ObjCObjectGraph::create(userData);

    _page->loadURLRequest(wkURLRequest.get(), wkUserData.get());
}

- (void)loadFileURL:(NSURL *)URL restrictToFilesWithin:(NSURL *)allowedDirectory
{
    [self loadFileURL:URL restrictToFilesWithin:allowedDirectory userData:nil];
}

- (void)loadFileURL:(NSURL *)URL restrictToFilesWithin:(NSURL *)allowedDirectory userData:(id)userData
{
    if (![URL isFileURL] || (allowedDirectory && ![allowedDirectory isFileURL]))
        [NSException raise:NSInvalidArgumentException format:@"Attempted to load a non-file URL"];

    RefPtr<ObjCObjectGraph> wkUserData;
    if (userData)
        wkUserData = ObjCObjectGraph::create(userData);

    _page->loadFile([URL _web_originalDataAsWTFString], [allowedDirectory _web_originalDataAsWTFString], wkUserData.get());
}

- (void)loadHTMLString:(NSString *)HTMLString baseURL:(NSURL *)baseURL
{
    [self loadHTMLString:HTMLString baseURL:baseURL userData:nil];
}

- (void)loadHTMLString:(NSString *)HTMLString baseURL:(NSURL *)baseURL userData:(id)userData
{
    RefPtr<ObjCObjectGraph> wkUserData;
    if (userData)
        wkUserData = ObjCObjectGraph::create(userData);

    _page->loadHTMLString(HTMLString, [baseURL _web_originalDataAsWTFString], wkUserData.get());
}

- (void)loadAlternateHTMLString:(NSString *)string baseURL:(NSURL *)baseURL forUnreachableURL:(NSURL *)unreachableURL
{
    _page->loadAlternateHTMLString(string, [baseURL _web_originalDataAsWTFString], [unreachableURL _web_originalDataAsWTFString]);
}

- (void)loadData:(NSData *)data MIMEType:(NSString *)MIMEType textEncodingName:(NSString *)encodingName baseURL:(NSURL *)baseURL
{
    [self loadData:data MIMEType:MIMEType textEncodingName:encodingName baseURL:baseURL userData:nil];
}

static void releaseNSData(unsigned char*, const void* data)
{
    [(NSData *)data release];
}

- (void)loadData:(NSData *)data MIMEType:(NSString *)MIMEType textEncodingName:(NSString *)encodingName baseURL:(NSURL *)baseURL userData:(id)userData
{
    RefPtr<WebData> wkData;
    if (data) {
        [data retain];
        wkData = WebData::createWithoutCopying((const unsigned char*)[data bytes], [data length], releaseNSData, data);
    }

    RefPtr<ObjCObjectGraph> wkUserData;
    if (userData)
        wkUserData = ObjCObjectGraph::create(userData);

    _page->loadData(wkData.get(), MIMEType, encodingName, [baseURL _web_originalDataAsWTFString], wkUserData.get());
}

- (void)stopLoading
{
    _page->stopLoading();
}

- (void)reload
{
    _page->reload(false);
}

- (void)reloadFromOrigin
{
    _page->reload(true);
}

#pragma mark Back/Forward

- (void)goForward
{
    _page->goForward();
}

- (BOOL)canGoForward
{
    return _page->canGoForward();
}

- (void)goBack
{
    _page->goBack();
}

- (BOOL)canGoBack
{
    return _page->canGoBack();
}

- (void)goToBackForwardListItem:(WKBackForwardListItem *)item
{
    _page->goToBackForwardItem(&item._item);
}

- (WKBackForwardList *)backForwardList
{
    return wrapper(_page->backForwardList());
}

#pragma mark Active Load Introspection

- (BOOL)isLoading
{
    return _page->pageLoadState().isLoading();
}

- (NSURL *)activeURL
{
    return [NSURL _web_URLWithWTFString:_page->pageLoadState().activeURL()];
}

- (NSURL *)provisionalURL
{
    return [NSURL _web_URLWithWTFString:_page->pageLoadState().provisionalURL()];
}

- (NSURL *)committedURL
{
    return [NSURL _web_URLWithWTFString:_page->pageLoadState().url()];
}

- (NSURL *)unreachableURL
{
    return [NSURL _web_URLWithWTFString:_page->pageLoadState().unreachableURL()];
}

- (double)estimatedProgress
{
    return _page->estimatedProgress();
}

#pragma mark Active Document Introspection

- (NSString *)title
{
    return _page->pageLoadState().title();
}

#pragma mark Zoom

- (CGFloat)textZoom
{
    return _page->textZoomFactor();
}

- (void)setTextZoom:(CGFloat)textZoom
{
    _page->setTextZoomFactor(textZoom);
}

- (CGFloat)pageZoom
{
    return _page->pageZoomFactor();
}

- (void)setPageZoom:(CGFloat)pageZoom
{
    _page->setPageZoomFactor(pageZoom);
}

static NSError *createErrorWithRecoveryAttempter(WKErrorRef wkError, WKFrameRef frame, WKBrowsingContextController *browsingContext)
{
    NSError *error = wrapper(*toImpl(wkError));
    NSMutableDictionary *userInfo = [NSMutableDictionary dictionaryWithObjectsAndKeys:
        browsingContext, WKRecoveryAttempterErrorKey,
        toImpl(frame)->wrapper(), frameErrorKey,
    nil];

    if (NSDictionary *originalUserInfo = error.userInfo)
        [userInfo addEntriesFromDictionary:originalUserInfo];

    return [[NSError alloc] initWithDomain:error.domain code:error.code userInfo:userInfo];
}

static void didStartProvisionalLoadForFrame(WKPageRef page, WKFrameRef frame, WKTypeRef userData, const void* clientInfo)
{
    if (!WKFrameIsMainFrame(frame))
        return;

    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if ([loadDelegate respondsToSelector:@selector(browsingContextControllerDidStartProvisionalLoad:)])
        [loadDelegate browsingContextControllerDidStartProvisionalLoad:browsingContext];
}

static void didReceiveServerRedirectForProvisionalLoadForFrame(WKPageRef page, WKFrameRef frame, WKTypeRef userData, const void* clientInfo)
{
    if (!WKFrameIsMainFrame(frame))
        return;

    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if ([loadDelegate respondsToSelector:@selector(browsingContextControllerDidReceiveServerRedirectForProvisionalLoad:)])
        [loadDelegate browsingContextControllerDidReceiveServerRedirectForProvisionalLoad:browsingContext];
}

static void didFailProvisionalLoadWithErrorForFrame(WKPageRef page, WKFrameRef frame, WKErrorRef error, WKTypeRef userData, const void* clientInfo)
{
    if (!WKFrameIsMainFrame(frame))
        return;

    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if ([loadDelegate respondsToSelector:@selector(browsingContextController:didFailProvisionalLoadWithError:)]) {
        RetainPtr<NSError> nsError = adoptNS(createErrorWithRecoveryAttempter(error, frame, browsingContext));
        [loadDelegate browsingContextController:browsingContext didFailProvisionalLoadWithError:nsError.get()];
    }
}

static void didCommitLoadForFrame(WKPageRef page, WKFrameRef frame, WKTypeRef userData, const void* clientInfo)
{
    if (!WKFrameIsMainFrame(frame))
        return;

    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if ([loadDelegate respondsToSelector:@selector(browsingContextControllerDidCommitLoad:)])
        [loadDelegate browsingContextControllerDidCommitLoad:browsingContext];
}

static void didFinishLoadForFrame(WKPageRef page, WKFrameRef frame, WKTypeRef userData, const void* clientInfo)
{
    if (!WKFrameIsMainFrame(frame))
        return;

    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if ([loadDelegate respondsToSelector:@selector(browsingContextControllerDidFinishLoad:)])
        [loadDelegate browsingContextControllerDidFinishLoad:browsingContext];
}

static void didFailLoadWithErrorForFrame(WKPageRef page, WKFrameRef frame, WKErrorRef error, WKTypeRef userData, const void* clientInfo)
{
    if (!WKFrameIsMainFrame(frame))
        return;

    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if ([loadDelegate respondsToSelector:@selector(browsingContextController:didFailLoadWithError:)]) {
        RetainPtr<NSError> nsError = adoptNS(createErrorWithRecoveryAttempter(error, frame, browsingContext));
        [loadDelegate browsingContextController:browsingContext didFailLoadWithError:nsError.get()];
    }
}

static bool canAuthenticateAgainstProtectionSpaceInFrame(WKPageRef page, WKFrameRef frame, WKProtectionSpaceRef protectionSpace, const void *clientInfo)
{
    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if ([loadDelegate respondsToSelector:@selector(browsingContextController:canAuthenticateAgainstProtectionSpace:)])
        return [(id <WKBrowsingContextLoadDelegatePrivate>)loadDelegate browsingContextController:browsingContext canAuthenticateAgainstProtectionSpace:wrapper(*toImpl(protectionSpace))];

    return false;
}

static void didReceiveAuthenticationChallengeInFrame(WKPageRef page, WKFrameRef frame, WKAuthenticationChallengeRef authenticationChallenge, const void *clientInfo)
{
    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if ([loadDelegate respondsToSelector:@selector(browsingContextController:didReceiveAuthenticationChallenge:)])
        [(id <WKBrowsingContextLoadDelegatePrivate>)loadDelegate browsingContextController:browsingContext didReceiveAuthenticationChallenge:wrapper(*toImpl(authenticationChallenge))];
}

static void didStartProgress(WKPageRef page, const void* clientInfo)
{
    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if ([loadDelegate respondsToSelector:@selector(browsingContextControllerDidStartProgress:)])
        [loadDelegate browsingContextControllerDidStartProgress:browsingContext];
}

static void didChangeProgress(WKPageRef page, const void* clientInfo)
{
    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if ([loadDelegate respondsToSelector:@selector(browsingContextController:estimatedProgressChangedTo:)])
        [loadDelegate browsingContextController:browsingContext estimatedProgressChangedTo:toImpl(page)->estimatedProgress()];
}

static void didFinishProgress(WKPageRef page, const void* clientInfo)
{
    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if ([loadDelegate respondsToSelector:@selector(browsingContextControllerDidFinishProgress:)])
        [loadDelegate browsingContextControllerDidFinishProgress:browsingContext];
}

static void didChangeBackForwardList(WKPageRef page, WKBackForwardListItemRef addedItem, WKArrayRef removedItems, const void *clientInfo)
{
    WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
    auto loadDelegate = browsingContext->_loadDelegate.get();

    if (![loadDelegate respondsToSelector:@selector(browsingContextControllerDidChangeBackForwardList:addedItem:removedItems:)])
        return;

    WKBackForwardListItem *added = addedItem ? wrapper(*toImpl(addedItem)) : nil;
    NSArray *removed = removedItems ? wrapper(*toImpl(removedItems)) : nil;
    [loadDelegate browsingContextControllerDidChangeBackForwardList:browsingContext addedItem:added removedItems:removed];
}

static void setUpPageLoaderClient(WKBrowsingContextController *browsingContext, WebPageProxy& page)
{
    WKPageLoaderClientV3 loaderClient;
    memset(&loaderClient, 0, sizeof(loaderClient));

    loaderClient.base.version = 3;
    loaderClient.base.clientInfo = browsingContext;
    loaderClient.didStartProvisionalLoadForFrame = didStartProvisionalLoadForFrame;
    loaderClient.didReceiveServerRedirectForProvisionalLoadForFrame = didReceiveServerRedirectForProvisionalLoadForFrame;
    loaderClient.didFailProvisionalLoadWithErrorForFrame = didFailProvisionalLoadWithErrorForFrame;
    loaderClient.didCommitLoadForFrame = didCommitLoadForFrame;
    loaderClient.didFinishLoadForFrame = didFinishLoadForFrame;
    loaderClient.didFailLoadWithErrorForFrame = didFailLoadWithErrorForFrame;

    loaderClient.canAuthenticateAgainstProtectionSpaceInFrame = canAuthenticateAgainstProtectionSpaceInFrame;
    loaderClient.didReceiveAuthenticationChallengeInFrame = didReceiveAuthenticationChallengeInFrame;

    loaderClient.didStartProgress = didStartProgress;
    loaderClient.didChangeProgress = didChangeProgress;
    loaderClient.didFinishProgress = didFinishProgress;
    loaderClient.didChangeBackForwardList = didChangeBackForwardList;

    page.initializeLoaderClient(&loaderClient.base);
}

static WKPolicyDecisionHandler makePolicyDecisionBlock(WKFramePolicyListenerRef listener)
{
    WKRetain(listener); // Released in the decision handler below.

    return [[^(WKPolicyDecision decision) {
        switch (decision) {
        case WKPolicyDecisionCancel:
            WKFramePolicyListenerIgnore(listener);                    
            break;
        
        case WKPolicyDecisionAllow:
            WKFramePolicyListenerUse(listener);
            break;
        
        case WKPolicyDecisionBecomeDownload:
            WKFramePolicyListenerDownload(listener);
            break;
        };

        WKRelease(listener); // Retained in the context above.
    } copy] autorelease];
}

static void setUpPagePolicyClient(WKBrowsingContextController *browsingContext, WebPageProxy& page)
{
    WKPagePolicyClientV1 policyClient;
    memset(&policyClient, 0, sizeof(policyClient));

    policyClient.base.version = 1;
    policyClient.base.clientInfo = browsingContext;

    policyClient.decidePolicyForNavigationAction = [](WKPageRef page, WKFrameRef frame, WKFrameNavigationType navigationType, WKEventModifiers modifiers, WKEventMouseButton mouseButton, WKFrameRef originatingFrame, WKURLRequestRef request, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
    {
        WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
        auto policyDelegate = browsingContext->_policyDelegate.get();

        if ([policyDelegate respondsToSelector:@selector(browsingContextController:decidePolicyForNavigationAction:decisionHandler:)]) {
            NSDictionary *actionDictionary = @{
                WKActionIsMainFrameKey: @(WKFrameIsMainFrame(frame)),
                WKActionNavigationTypeKey: @(navigationType),
                WKActionModifierFlagsKey: @(modifiers),
                WKActionMouseButtonKey: @(mouseButton),
                WKActionURLRequestKey: adoptNS(WKURLRequestCopyNSURLRequest(request)).get()
            };

            if (originatingFrame) {
                actionDictionary = [[actionDictionary mutableCopy] autorelease];
                [(NSMutableDictionary *)actionDictionary setObject:[NSURL _web_URLWithWTFString:toImpl(originatingFrame)->url()] forKey:WKActionOriginatingFrameURLKey];
            }
            
            [policyDelegate browsingContextController:browsingContext decidePolicyForNavigationAction:actionDictionary decisionHandler:makePolicyDecisionBlock(listener)];
        } else
            WKFramePolicyListenerUse(listener);
    };

    policyClient.decidePolicyForNewWindowAction = [](WKPageRef page, WKFrameRef frame, WKFrameNavigationType navigationType, WKEventModifiers modifiers, WKEventMouseButton mouseButton, WKURLRequestRef request, WKStringRef frameName, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
    {
        WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
        auto policyDelegate = browsingContext->_policyDelegate.get();

        if ([policyDelegate respondsToSelector:@selector(browsingContextController:decidePolicyForNewWindowAction:decisionHandler:)]) {
            NSDictionary *actionDictionary = @{
                WKActionIsMainFrameKey: @(WKFrameIsMainFrame(frame)),
                WKActionNavigationTypeKey: @(navigationType),
                WKActionModifierFlagsKey: @(modifiers),
                WKActionMouseButtonKey: @(mouseButton),
                WKActionURLRequestKey: adoptNS(WKURLRequestCopyNSURLRequest(request)).get(),
                WKActionFrameNameKey: toImpl(frameName)->wrapper()
            };
            
            [policyDelegate browsingContextController:browsingContext decidePolicyForNewWindowAction:actionDictionary decisionHandler:makePolicyDecisionBlock(listener)];
        } else
            WKFramePolicyListenerUse(listener);
    };

    policyClient.decidePolicyForResponse = [](WKPageRef page, WKFrameRef frame, WKURLResponseRef response, WKURLRequestRef request, bool canShowMIMEType, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
    {
        WKBrowsingContextController *browsingContext = (WKBrowsingContextController *)clientInfo;
        auto policyDelegate = browsingContext->_policyDelegate.get();

        if ([policyDelegate respondsToSelector:@selector(browsingContextController:decidePolicyForResponseAction:decisionHandler:)]) {
            NSDictionary *actionDictionary = @{
                WKActionIsMainFrameKey: @(WKFrameIsMainFrame(frame)),
                WKActionURLRequestKey: adoptNS(WKURLRequestCopyNSURLRequest(request)).get(),
                WKActionURLResponseKey: adoptNS(WKURLResponseCopyNSURLResponse(response)).get(),
                WKActionCanShowMIMETypeKey: @(canShowMIMEType),
            };

            [policyDelegate browsingContextController:browsingContext decidePolicyForResponseAction:actionDictionary decisionHandler:makePolicyDecisionBlock(listener)];
        } else
            WKFramePolicyListenerUse(listener);
    };

    page.initializePolicyClient(&policyClient.base);
}

- (id <WKBrowsingContextLoadDelegate>)loadDelegate
{
    return _loadDelegate.getAutoreleased();
}

- (void)setLoadDelegate:(id <WKBrowsingContextLoadDelegate>)loadDelegate
{
    _loadDelegate = loadDelegate;

    if (loadDelegate)
        setUpPageLoaderClient(self, *_page);
    else
        _page->initializeLoaderClient(nullptr);
}

- (id <WKBrowsingContextPolicyDelegate>)policyDelegate
{
    return _policyDelegate.getAutoreleased();
}

- (void)setPolicyDelegate:(id <WKBrowsingContextPolicyDelegate>)policyDelegate
{
    _policyDelegate = policyDelegate;

    if (policyDelegate)
        setUpPagePolicyClient(self, *_page);
    else
        _page->initializePolicyClient(nullptr);
}

- (id <WKBrowsingContextHistoryDelegate>)historyDelegate
{
    return _historyDelegate.getAutoreleased();
}

- (void)setHistoryDelegate:(id <WKBrowsingContextHistoryDelegate>)historyDelegate
{
    _historyDelegate = historyDelegate;
}

+ (NSMutableSet *)customSchemes
{
    static NSMutableSet *customSchemes = [[NSMutableSet alloc] init];
    return customSchemes;
}

#pragma mark WKErrorRecoveryAttempting

- (BOOL)attemptRecoveryFromError:(NSError *)error
{
    NSDictionary *userInfo = error.userInfo;

    NSString *failingURLString = userInfo[NSURLErrorFailingURLStringErrorKey];
    if (!failingURLString)
        return NO;

    NSObject <WKObject> *frame = userInfo[frameErrorKey];
    if (![frame conformsToProtocol:@protocol(WKObject)])
        return NO;

    if (frame._apiObject.type() != API::Object::Type::Frame)
        return NO;

    WebFrameProxy& webFrame = *static_cast<WebFrameProxy*>(&frame._apiObject);
    webFrame.loadURL(failingURLString);

    return YES;
}

#pragma mark WKObject protocol implementation

- (API::Object&)_apiObject
{
    return *reinterpret_cast<API::Object*>(&_page);
}

@end

@implementation WKBrowsingContextController (Private)

- (WKPageRef)_pageRef
{
    return toAPI(_page.get());
}

- (void)setPaginationMode:(WKBrowsingContextPaginationMode)paginationMode
{
    Pagination::Mode mode;
    switch (paginationMode) {
    case WKPaginationModeUnpaginated:
        mode = Pagination::Unpaginated;
        break;
    case WKPaginationModeLeftToRight:
        mode = Pagination::LeftToRightPaginated;
        break;
    case WKPaginationModeRightToLeft:
        mode = Pagination::RightToLeftPaginated;
        break;
    case WKPaginationModeTopToBottom:
        mode = Pagination::TopToBottomPaginated;
        break;
    case WKPaginationModeBottomToTop:
        mode = Pagination::BottomToTopPaginated;
        break;
    default:
        return;
    }

    _page->setPaginationMode(mode);
}

- (WKBrowsingContextPaginationMode)paginationMode
{
    switch (_page->paginationMode()) {
    case Pagination::Unpaginated:
        return WKPaginationModeUnpaginated;
    case Pagination::LeftToRightPaginated:
        return WKPaginationModeLeftToRight;
    case Pagination::RightToLeftPaginated:
        return WKPaginationModeRightToLeft;
    case Pagination::TopToBottomPaginated:
        return WKPaginationModeTopToBottom;
    case Pagination::BottomToTopPaginated:
        return WKPaginationModeBottomToTop;
    }

    ASSERT_NOT_REACHED();
    return WKPaginationModeUnpaginated;
}

- (void)setPaginationBehavesLikeColumns:(BOOL)behavesLikeColumns
{
    _page->setPaginationBehavesLikeColumns(behavesLikeColumns);
}

- (BOOL)paginationBehavesLikeColumns
{
    return _page->paginationBehavesLikeColumns();
}

- (void)setPageLength:(CGFloat)pageLength
{
    _page->setPageLength(pageLength);
}

- (CGFloat)pageLength
{
    return _page->pageLength();
}

- (void)setGapBetweenPages:(CGFloat)gapBetweenPages
{
    _page->setGapBetweenPages(gapBetweenPages);
}

- (CGFloat)gapBetweenPages
{
    return _page->gapBetweenPages();
}

- (NSUInteger)pageCount
{
    return _page->pageCount();
}

- (WKBrowsingContextHandle *)handle
{
    return [[[WKBrowsingContextHandle alloc] _initWithPageID:_page->pageID()] autorelease];
}

@end

#endif // WK_API_ENABLED
