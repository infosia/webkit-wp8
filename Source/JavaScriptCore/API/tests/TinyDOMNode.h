#import <JavaScriptCore/JavaScriptCore.h>

@class TinyDOMNode;

@protocol TinyDOMNode <JSExport>

- (void)appendChild:(TinyDOMNode *)child;
- (NSUInteger)numberOfChildren;
- (TinyDOMNode *)childAtIndex:(NSUInteger)index;
- (void)removeChildAtIndex:(NSUInteger)index;

@end


@interface TinyDOMNode : NSObject<TinyDOMNode>

+ (JSVirtualMachine *)sharedVirtualMachine;

//@property (nonatomic, readonly) JSVirtualMachine *virtualMachine;
@property (nonatomic, readonly) JSContext *context;

@end
