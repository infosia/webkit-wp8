#import <JavaScriptCore/JavaScriptCore.h>

@protocol ParentObject <JSExport>
@end


@interface ParentObject : NSObject<ParentObject>

+ (NSString *)parentTest;

@end
