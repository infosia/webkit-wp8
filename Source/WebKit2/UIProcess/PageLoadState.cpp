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

#include "config.h"
#include "PageLoadState.h"

namespace WebKit {

PageLoadState::PageLoadState()
    : m_state(State::Finished)
    , m_estimatedProgress(0)
{
}

PageLoadState::~PageLoadState()
{
    ASSERT(m_observers.isEmpty());
}

void PageLoadState::addObserver(Observer& observer)
{
    ASSERT(!m_observers.contains(&observer));

    m_observers.append(&observer);
}

void PageLoadState::removeObserver(Observer& observer)
{
    ASSERT(m_observers.contains(&observer));

    size_t index = m_observers.find(&observer);
    m_observers.remove(index);
}

void PageLoadState::reset()
{
    setState(State::Finished);

    m_pendingAPIRequestURL = String();
    m_provisionalURL = String();
    m_url = String();

    m_unreachableURL = String();
    m_lastUnreachableURL = String();

    callObserverCallback(&Observer::willChangeTitle);
    m_title = String();
    callObserverCallback(&Observer::didChangeTitle);

    callObserverCallback(&Observer::willChangeEstimatedProgress);
    m_estimatedProgress = 0;
    callObserverCallback(&Observer::didChangeEstimatedProgress);
}

bool PageLoadState::isLoading() const
{
    return isLoadingState(m_state);
}

String PageLoadState::activeURL() const
{
    // If there is a currently pending URL, it is the active URL,
    // even when there's no main frame yet, as it might be the
    // first API request.
    if (!m_pendingAPIRequestURL.isNull())
        return m_pendingAPIRequestURL;

    if (!m_unreachableURL.isEmpty())
        return m_unreachableURL;

    switch (m_state) {
    case State::Provisional:
        return m_provisionalURL;
    case State::Committed:
    case State::Finished:
        return m_url;
    }

    ASSERT_NOT_REACHED();
    return String();
}

// Always start progress at initialProgressValue. This helps provide feedback as
// soon as a load starts.

static const double initialProgressValue = 0.1;

double PageLoadState::estimatedProgress() const
{
    if (!m_pendingAPIRequestURL.isNull())
        return initialProgressValue;

    return m_estimatedProgress;
}

const String& PageLoadState::pendingAPIRequestURL() const
{
    return m_pendingAPIRequestURL;
}

void PageLoadState::setPendingAPIRequestURL(const String& pendingAPIRequestURL)
{
    callObserverCallback(&Observer::willChangeEstimatedProgress);
    m_pendingAPIRequestURL = pendingAPIRequestURL;
    callObserverCallback(&Observer::didChangeEstimatedProgress);
}

void PageLoadState::clearPendingAPIRequestURL()
{
    callObserverCallback(&Observer::willChangeEstimatedProgress);
    m_pendingAPIRequestURL = String();
    callObserverCallback(&Observer::didChangeEstimatedProgress);
}

void PageLoadState::didStartProvisionalLoad(const String& url, const String& unreachableURL)
{
    ASSERT(m_provisionalURL.isEmpty());

    setState(State::Provisional);

    m_provisionalURL = url;

    setUnreachableURL(unreachableURL);
}

void PageLoadState::didReceiveServerRedirectForProvisionalLoad(const String& url)
{
    ASSERT(m_state == State::Provisional);

    m_provisionalURL = url;
}

void PageLoadState::didFailProvisionalLoad()
{
    ASSERT(m_state == State::Provisional);

    setState(State::Finished);

    m_provisionalURL = String();
    m_unreachableURL = m_lastUnreachableURL;
}

void PageLoadState::didCommitLoad()
{
    ASSERT(m_state == State::Provisional);

    setState(State::Committed);

    m_url = m_provisionalURL;
    m_provisionalURL = String();

    m_title = String();
}

void PageLoadState::didFinishLoad()
{
    ASSERT(m_state == State::Committed);
    ASSERT(m_provisionalURL.isEmpty());

    setState(State::Finished);
}

void PageLoadState::didFailLoad()
{
    ASSERT(m_provisionalURL.isEmpty());

    setState(State::Finished);
}

void PageLoadState::didSameDocumentNavigation(const String& url)
{
    ASSERT(!m_url.isEmpty());

    m_url = url;
}

void PageLoadState::setUnreachableURL(const String& unreachableURL)
{
    m_lastUnreachableURL = m_unreachableURL;
    m_unreachableURL = unreachableURL;
}

const String& PageLoadState::title() const
{
    return m_title;
}

void PageLoadState::setTitle(const String& title)
{
    callObserverCallback(&Observer::willChangeTitle);
    m_title = title;
    callObserverCallback(&Observer::didChangeTitle);
}

void PageLoadState::didStartProgress()
{
    callObserverCallback(&Observer::willChangeEstimatedProgress);
    m_estimatedProgress = initialProgressValue;
    callObserverCallback(&Observer::didChangeEstimatedProgress);
}

void PageLoadState::didChangeProgress(double value)
{
    callObserverCallback(&Observer::willChangeEstimatedProgress);
    m_estimatedProgress = value;
    callObserverCallback(&Observer::didChangeEstimatedProgress);
}

void PageLoadState::didFinishProgress()
{
    callObserverCallback(&Observer::willChangeEstimatedProgress);
    m_estimatedProgress = 1;
    callObserverCallback(&Observer::didChangeEstimatedProgress);
}

bool PageLoadState::isLoadingState(State state)
{
    switch (state) {
    case State::Provisional:
    case State::Committed:
        return true;

    case State::Finished:
        return false;
    }

    ASSERT_NOT_REACHED();
    return false;
}

void PageLoadState::setState(State state)
{
    if (m_state == state)
        return;

    bool isLoadingIsChanging = false;

    if (isLoadingState(m_state) != isLoadingState(state))
        isLoadingIsChanging = true;

    if (isLoadingIsChanging)
        callObserverCallback(&Observer::willChangeIsLoading);

    m_state = state;

    if (isLoadingIsChanging)
        callObserverCallback(&Observer::didChangeIsLoading);
}

void PageLoadState::callObserverCallback(void (Observer::*callback)())
{
    for (auto* observer : m_observers)
        (observer->*callback)();
}

} // namespace WebKit
