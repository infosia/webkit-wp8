#import "TextXYZ.h"

@interface TextXYZ ()
@property (readwrite) bool tested;
@end


@implementation TextXYZ {
    JSManagedValue *m_weakOnclickHandler;
    JSManagedValue *m_onclickHandler;
}

@synthesize x = _x;
@synthesize y = _y;
@synthesize z = _z;


- (id)init
{
    self = [super init];
    if (!self) {
        return nil;
    }
    
    _tested = false;
    return self;
}


- (void)test:(NSString *)message {
    self.tested = [message isEqual:@"test"] && self.x == 13 & self.y == 4 && self.z == 5;
}

- (void)setWeakOnclick:(JSValue *)value {
    m_weakOnclickHandler = [JSManagedValue managedValueWithValue:value];
}

- (void)setOnclick:(JSValue *)value {
    m_onclickHandler = [JSManagedValue managedValueWithValue:value];
    [value.context.virtualMachine addManagedReference:m_onclickHandler withOwner:self];
}

- (JSValue *)weakOnclick {
    return [m_weakOnclickHandler value];
}

- (JSValue *)onclick {
    return [m_onclickHandler value];
}

- (void)click {
    if (!m_onclickHandler)
        return;
    
    JSValue *function = [m_onclickHandler value];
    [function callWithArguments:[NSArray array]];
}

- (void)dealloc {
    [[m_onclickHandler value].context.virtualMachine removeManagedReference:m_onclickHandler withOwner:self];
}

@end
