#import "ParentObject.h"

@protocol TestObject <JSExport>

@property int variable;
@property (readonly) int six;
@property CGPoint point;

+ (NSString *)classTest;
+ (NSString *)parentTest;
- (NSString *)getString;

JSExportAs(testArgumentTypes,
           - (NSString *)testArgumentTypesWithInt:(int)i double:(double)d boolean:(BOOL)b string:(NSString *)s number:(NSNumber *)n array:(NSArray *)a dictionary:(NSDictionary *)o
           );

- (void)callback:(JSValue *)function;
- (void)bogusCallback:(void(^)(int))function;

@end


@interface TestObject : ParentObject <TestObject>

@property int six;
+ (id)testObject;

@end
