//
//  Foo.m
//  JavaScriptCore_API
//
//  Created by Matt Langston on 8/8/13.
//  Copyright (c) 2013 Appcelerator. All rights reserved.
//

#import "Foo.h"

#ifdef APPCELERATOR_OO_API_ENABLED
#include "AppcFoo.h"
#endif

@implementation Foo

- (id)init
{
    self = [super init];
    if (!self) {
        return nil;
    }
    
#ifdef APPCELERATOR_OO_API_ENABLED
    _fourtyTwo = AppcFooFourtyTwo();
#else
    NSLog(@"Hello from Objective-C");
    _fourtyTwo = 42;
    _array = @[@42];
    _dictionary = @{@"fourtyTwo": @42};
#endif
    
    return self;
}

//- (int) fourtyTwo {
//    int returnValue = -1;
//#ifdef APPCELERATOR_OO_API_ENABLED
//    returnValue = AppcFooFourtyTwo();
//#else
//    NSLog(@"Hello from Objective-C");
//    returnValue = 42;
//#endif
//    return returnValue;
//}
//
//- (NSArray *)array {
//
//}
//
//- (NSDictionary *)dictionary {
//
//}

- (void)doIt {
}

@end
