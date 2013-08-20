//
//  TinyDOMNode.m
//  titanium
//
//  Created by Matt Langston on 7/21/13.
//  Copyright (c) 2013 Appcelerator. All rights reserved.
//

#import "TinyDOMNode.h"

@interface TinyDOMNode ()

//@property (nonatomic, readwrite) JSVirtualMachine *virtualMachine;
@property (nonatomic, readwrite) JSContext *context;

@end

@implementation TinyDOMNode {
    NSMutableArray *m_children;
}

+ (JSVirtualMachine *)sharedVirtualMachine
{
    static dispatch_once_t once;
    static id sharedVirtualMachine;
    dispatch_once(&once, ^{
        sharedVirtualMachine = [[JSVirtualMachine alloc] init];
    });
    return sharedVirtualMachine;
}

- (id)init
{
    self = [super init];
    if (!self) {
        return nil;
    }
    
//    _virtualMachine = [[JSVirtualMachine alloc] init];
    _context = [[JSContext alloc] initWithVirtualMachine:[TinyDOMNode sharedVirtualMachine]];
    m_children = [[NSMutableArray alloc] initWithCapacity:0];
    
    return self;
}

- (void)dealloc
{
    NSEnumerator *enumerator = [m_children objectEnumerator];
    id nextChild;
    while ((nextChild = [enumerator nextObject])) {
        [[TinyDOMNode sharedVirtualMachine] removeManagedReference:nextChild withOwner:self];
    }
    
#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

- (void)appendChild:(TinyDOMNode *)child
{
    [[TinyDOMNode sharedVirtualMachine] addManagedReference:child withOwner:self];
    [m_children addObject:child];
}

- (NSUInteger)numberOfChildren
{
    return [m_children count];
}

- (TinyDOMNode *)childAtIndex:(NSUInteger)index
{
    if (index >= [m_children count]) {
        return nil;
    }
    return [m_children objectAtIndex:index];
}

- (void)removeChildAtIndex:(NSUInteger)index
{
    if (index >= [m_children count]) {
        return;
    }
    [[TinyDOMNode sharedVirtualMachine] removeManagedReference:[m_children objectAtIndex:index] withOwner:self];
    [m_children removeObjectAtIndex:index];
}

@end
