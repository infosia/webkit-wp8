#import <JavaScriptCore/JavaScriptCore.h>

@protocol TextXYZ <JSExport>

@property int x;
@property (readonly) int y;
@property (assign) JSValue *onclick;
@property (assign) JSValue *weakOnclick;

- (void)test:(NSString *)message;

@end


@interface TextXYZ : NSObject <TextXYZ>

@property int x;
@property int y;
@property int z;

@property (readonly, getter = isTested) bool tested;

- (void)click;

@end
