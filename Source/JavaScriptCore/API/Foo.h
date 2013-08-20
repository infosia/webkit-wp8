//
//  Foo.h
//  JavaScriptCore_API
//
//  Created by Matt Langston on 8/8/13.
//  Copyright (c) 2013 Appcelerator. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Foo : NSObject

@property (readonly) int fourtyTwo;
@property (readonly) NSArray *array;
@property (readonly) NSDictionary *dictionary;

- (void)doIt;

@end
