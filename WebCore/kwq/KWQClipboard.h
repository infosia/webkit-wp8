/*
 * Copyright (C) 2004 Apple Computer, Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

// An implementation of the clipboard class from IE that talks to the Cocoa Pasteboard

#ifndef KWQCLIPBOARD_H_
#define KWQCLIPBOARD_H_

#import <AppKit/AppKit.h>
#include "xml/dom2_eventsimpl.h"

class KWQClipboard : public DOM::ClipboardImpl
{
public:
    // security mechanism
    typedef enum {
        Numb, Writable, TypesReadable, Readable
    } AccessPolicy;

    KWQClipboard(bool forDragging, NSPasteboard *pasteboard, AccessPolicy policy);
    virtual ~KWQClipboard();

    bool isForDragging() const;
    
    DOM::DOMString dropEffect() const;
    void setDropEffect(const DOM::DOMString &s);
    DOM::DOMString effectAllowed() const;
    void setEffectAllowed(const DOM::DOMString &s);
    
    void clearData(const DOM::DOMString &type);
    void clearAllData();
    DOM::DOMString getData(const DOM::DOMString &type, bool &success) const;
    bool setData(const DOM::DOMString &type, const DOM::DOMString &data);
        
    // extensions beyond IE's API
    virtual QStringList types() const;

    QPoint dragLocation() const;
    void setDragLocation(const QPoint &);
    QPixmap dragImage() const;
    void setDragImage(const QPixmap &);

    // Methods for getting info in Cocoa's type system
    NSImage *dragNSImage();
    bool sourceOperation(NSDragOperation *op) const;
    bool destinationOperation(NSDragOperation *op) const;
    void setSourceOperation(NSDragOperation op);
    void setDestinationOperation(NSDragOperation op);

    // sets AccessPolicy = Numb - trap door, once this is set, no going back
    void becomeNumb();

private:
    NSPasteboard *m_pasteboard;
    bool m_forDragging;
    DOM::DOMString m_dropEffect;
    DOM::DOMString m_effectAllowed;
    QPoint m_dragLoc;
    QPixmap m_dragImage;
    AccessPolicy m_policy;
    int m_changeCount;
};


#endif
