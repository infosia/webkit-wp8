/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 * Copyright (C) 2014 Apple Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(VIDEO_TRACK) && ENABLE(WEBVTT_REGIONS)

#include "TextTrackRegionList.h"

namespace WebCore {

TextTrackRegionList::TextTrackRegionList()
{
}

unsigned long TextTrackRegionList::length() const
{
    return m_list.size();
}

TextTrackRegion* TextTrackRegionList::item(unsigned index) const
{
    if (index < m_list.size())
        return m_list[index].get();

    return 0;
}

TextTrackRegion* TextTrackRegionList::getRegionById(const String& id) const
{
    if (id.isEmpty())
        return 0;

    for (const auto& region : m_list) {
        if (region->id() == id)
            return region.get();
    }

    return 0;
}

void TextTrackRegionList::add(PassRefPtr<TextTrackRegion> region)
{
    m_list.append(region);
}

bool TextTrackRegionList::remove(TextTrackRegion* region)
{
    size_t index = m_list.find(region);
    if (index == notFound)
        return false;

    m_list.remove(index);
    return true;
}

void TextTrackRegionList::clear()
{
    m_list.clear();
}

} // namespace WebCore

#endif
