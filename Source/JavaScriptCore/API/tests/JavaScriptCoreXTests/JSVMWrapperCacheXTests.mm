//
//  NSMapTableTests.m
//  API
//
//  Created by Matt Langston on 8/27/13.
//  Copyright (c) 2013 Appcelerator. All rights reserved.
//

#import <XCTest/XCTest.h>
#include "API/JSVMWrapperCacheXRef.h"

@interface JSVMWrapperCacheXTests : XCTestCase

@end

@implementation JSVMWrapperCacheXTests

- (void)setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testJSVMWrapperCacheX
{
    JSVMWrapperCacheXRef wrapperCache = JSVMWrapperCacheXCreate();
    XCTAssertTrue(wrapperCache);
    
    //    JSVMWrapperCacheXRetain(wrapperCache);
    JSVMWrapperCacheXRelease(wrapperCache);
    //    JSVMWrapperCacheXRelease(wrapperCache);
    //    JSVMWrapperCacheXRelease(jsVirtualMachineX);
    //XCTAssertFalse(wrapperCache);
}

@end
