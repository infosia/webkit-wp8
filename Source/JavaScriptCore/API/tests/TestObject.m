#import "TestObject.h"

@implementation TestObject

@synthesize variable = _variable;
@synthesize six = _six;
@synthesize point = _point;

+ (id)testObject {
    return [[TestObject alloc] init];
}

+ (NSString *)classTest {
    return @"classTest - okay";
}

- (NSString *)getString {
    return @"42";
}

- (NSString *)testArgumentTypesWithInt:(int)i double:(double)d boolean:(BOOL)b string:(NSString *)s number:(NSNumber *)n array:(NSArray *)a dictionary:(NSDictionary *)o {
    return [NSString stringWithFormat:@"%d,%g,%d,%@,%d,%@,%@", i, d, b==YES?true:false,s,[n intValue],a[1],o[@"x"]];
}

- (void)callback:(JSValue *)function {
    [function callWithArguments:[NSArray arrayWithObject:[NSNumber numberWithInt:42]]];
}

- (void)bogusCallback:(void(^)(int))function {
    function(42);
}

@end
