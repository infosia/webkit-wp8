//
//  JavaScriptCore_API_Tests.m
//  JavaScriptCore_API_Tests
//
//  Created by Matt Langston on 8/6/13.
//  Copyright (c) 2013 Appcelerator. All rights reserved.
//

#import <XCTest/XCTest.h>
//#import <JavaScriptCore/JavaScriptCore.h>
#import "Foo.h"


@interface FooTests : XCTestCase

@end

@implementation FooTests

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

- (void)test_Foo {
    // To have the Objective-C class Foo use the AppcFoo C++ implementation, add
    // -DAPPC_API_ENABLED to the compiler flags of Foo.mm in
    // "Build Phases > Compile Sources > Compiler Flags"

    Foo *foo = [[Foo alloc] init];
    XCTAssertNotNil(foo);
    XCTAssertEqual(42, foo.fourtyTwo);
    XCTAssertNotNil(foo.array);
    XCTAssertEqual(1UL, foo.array.count);
    XCTAssertEqualObjects(@42, foo.array[0]);
    XCTAssertNotNil(foo.dictionary);
    XCTAssertEqual(1UL, foo.dictionary.count);
    //XCTAssertTrue(foo.dictionary.);
}


@end
