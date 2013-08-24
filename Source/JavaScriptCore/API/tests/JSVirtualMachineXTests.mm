//
//  JSVirtualMachineXTests.m
//  API
//
//  Created by Matt Langston on 8/23/13.
//  Copyright (c) 2013 Appcelerator. All rights reserved.
//

#import <XCTest/XCTest.h>
#include <JavaScriptCore/JavaScriptCoreX.h>

@interface JSVirtualMachineXTests : XCTestCase

@end

@implementation JSVirtualMachineXTests

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

- (void)testJSVirtualMachineX
{
    JSVirtualMachineXRef jsVirtualMachineX = JSVirtualMachineXCreate();
    XCTAssertTrue(jsVirtualMachineX);
    
//    JSVirtualMachineXRetain(jsVirtualMachineX);
    JSVirtualMachineXRelease(jsVirtualMachineX);
//    JSVirtualMachineXRelease(jsVirtualMachineX);
//    JSVirtualMachineXRelease(jsVirtualMachineX);
    //XCTAssertFalse(jsVirtualMachineX);
}

@end
