//
//  ObjectiveCAPITests.mm
//
//  Created by Matt Langston on 7/21/13.
//  Copyright (c) 2013 Appcelerator. All rights reserved.
//

#import <XCTest/XCTest.h>

#import <JavaScriptCore/JavaScriptCore.h>
#import "TestObject.h"
#import "TextXYZ.h"
#import "TinyDOMNode.h"

extern "C" void JSSynchronousGarbageCollectForDebugging(JSContextRef);
extern "C" bool _Block_has_signature(id);
extern "C" const char * _Block_signature(id);

@interface ObjectiveCAPITests : XCTestCase
@end

@implementation ObjectiveCAPITests {
    JSContext *context;
}

- (void)setUp
{
    [super setUp];
    // Put setup code here; it will be run once, before the first test case.
    context = [[JSContext alloc] init];
}

- (void)tearDown
{
    // Put teardown code here; it will be run once, after the last test case.
    [super tearDown];
}

- (void)test_evaluateScript_1 {
    JSValue *result = [context evaluateScript:@"2 + 2"];
    XCTAssertTrue([result isNumber]);
    XCTAssertEqual(4, [result toInt32]);
}

- (void)test_evaluateScript_2 {
    NSString *result = [NSString stringWithFormat:@"Two plus two is %@", [context evaluateScript:@"2 + 2"]];
    XCTAssertEqualObjects(@"Two plus two is 4", result);
}

- (void)test_global_object_1 {
    context[@"message"] = @"Hello";
    JSValue *result = [context evaluateScript:@"message + ', World!'"];
    XCTAssertTrue([result isString]);
    XCTAssertEqualObjects(@"Hello, World!", [result toString]);
}

- (void)test_global_object_2 {
    JSValue *result = [context evaluateScript:@"({ x:42 })"];
    XCTAssertTrue([result isObject]);
    XCTAssertEqualObjects(@42, [result[@"x"] toNumber]);
    
    id obj = [result toObject];
    // Check dictionary literal
    XCTAssertTrue([obj isKindOfClass:[NSDictionary class]]);
    
    id num = obj[@"x"];
    // Check numeric literal.
    XCTAssertTrue([num isKindOfClass:[NSNumber class]]);
}

- (void)test_BlockSignature {
    static bool containsClass = ^{
        id block = ^(NSString *string){ return string; };
        return _Block_has_signature(block) && strstr(_Block_signature(block), "NSString");
    } ();
    XCTAssertTrue(containsClass);
}

- (void)test_blockCallback_1 {
    __block int result;
    context[@"blockCallback"] = ^(int value){
        result = value;
    };
    [context evaluateScript:@"blockCallback(42)"];
    XCTAssertEqual(42, result);
}

- (void)test_blockCallback_2 {
    __block bool result = false;
    context[@"blockCallback"] = ^(NSString *value){
        result = [@"42" isEqualToString:value] == YES;
    };
    [context evaluateScript:@"blockCallback(42)"];
    XCTAssertTrue(result);
}

- (void)test_exception_1 {
    XCTAssertFalse(context.exception);
    
    [context evaluateScript:@"!@#$%^&*() THIS IS NOT VALID JAVASCRIPT SYNTAX !@#$%^&*()"];
    XCTAssertTrue(context.exception);
}

- (void)test_exceptionHandler_1 {
    __block bool caught = false;
    context.exceptionHandler = ^(JSContext *context_, JSValue *exception_) {
        (void)context_;
        (void)exception_;
        caught = true;
    };
    [context evaluateScript:@"!@#$%^&*() THIS IS NOT VALID JAVASCRIPT SYNTAX !@#$%^&*()"];
    XCTAssertTrue(caught);
}

- (void)test_explicit_throw_in_callback {
    context[@"callback"] = ^{
        JSContext *currentContext = [JSContext currentContext];
        currentContext.exception = [JSValue valueWithNewErrorFromMessage:@"Something went wrong." inContext:currentContext];
    };
    JSValue *result = [context evaluateScript:@"var result; try { callback(); } catch (e) { result = 'Caught exception'; }"];
    // Explicit throw in callback - was caught by JavaScript
    XCTAssertEqualObjects(@"Caught exception", [result toString]);
    
    // Explicit throw in callback - not thrown to Objective-C
    XCTAssertFalse(context.exception);
}

- (void)test_implicit_throw_in_callback {
    context[@"callback"] = ^{
        JSContext *currentContext = [JSContext currentContext];
        [currentContext evaluateScript:@"!@#$%^&*() THIS IS NOT VALID JAVASCRIPT SYNTAX !@#$%^&*()"];
    };
    JSValue *result = [context evaluateScript:@"var result; try { callback(); } catch (e) { result = 'Caught exception'; }"];
    // Implicit throw in callback - was caught by JavaScript
    XCTAssertEqualObjects(@"Caught exception", [result toString]);
    
    // Implicit throw in callback - not thrown to Objective-C
    XCTAssertFalse(context.exception);
}

- (void)test_sumFunction {
    [context evaluateScript:
     @"function sum(array) { \
     var result = 0; \
     for (var i in array) \
     result += array[i]; \
     return result; \
     }"];
    JSValue *array = [JSValue valueWithObject:@[@13, @2, @7] inContext:context];
    JSValue *sumFunction = context[@"sum"];
    JSValue *result = [sumFunction callWithArguments:@[ array ]];
    // sum([13, 2, 7])
    XCTAssertEqual(22, [result toInt32]);
}

- (void)test_mulAddFunction {
    JSValue *mulAddFunction = [context evaluateScript:
                               @"(function(array, object) { \
                               var result = []; \
                               for (var i in array) \
                               result.push(array[i] * object.x + object.y); \
                               return result; \
                               })"];
    JSValue *result = [mulAddFunction callWithArguments:@[ @[ @2, @4, @8 ], @{ @"x":@0.5, @"y":@42 } ]];
    // mulAddFunction
    XCTAssertTrue([result isObject]);
    XCTAssertEqualObjects(@"43,44,46", [result toString]);
}

- (void)test_Array {
    JSValue *array = [JSValue valueWithNewArrayInContext:context];
    // arrayLengthEmpty
    // TODO: (MDL) Why is this test failing?
//    XCTAssertEqual(0U, [[array[@"length"] toNumber] unsignedIntegerValue]);
    
    JSValue *value1 = [JSValue valueWithInt32:42 inContext:context];
    JSValue *value2 = [JSValue valueWithInt32:24 inContext:context];
    NSUInteger lowIndex = 5;
    NSUInteger maxLength = UINT_MAX;
    
    [array setValue:value1 atIndex:lowIndex];
    // array.length after put to low index
    XCTAssertEqual((lowIndex + 1), [[array[@"length"] toNumber] unsignedIntegerValue]);
    
    [array setValue:value1 atIndex:(maxLength - 1)];
    // array.length after put to maxLength - 1
    XCTAssertEqual(maxLength, [[array[@"length"] toNumber] unsignedIntegerValue]);
    
    [array setValue:value2 atIndex:maxLength];
    // array.length after put to maxLength
    XCTAssertEqual(maxLength, [[array[@"length"] toNumber] unsignedIntegerValue]);
    
    [array setValue:value2 atIndex:(maxLength + 1)];
    // array.length after put to maxLength + 1
    XCTAssertEqual(maxLength, [[array[@"length"] toNumber] unsignedIntegerValue]);
    
    // valueAtIndex:0 is undefined
    // TODO: (MDL) Why isn't this undefined?
    //        XCTAssertTrue([[array valueAtIndex:0] isUndefined]);
    // valueAtIndex:lowIndex
    XCTAssertEqual(42, [[array valueAtIndex:lowIndex] toInt32]);
    // valueAtIndex:maxLength - 1
    XCTAssertEqual(42, [[array valueAtIndex:(maxLength - 1)] toInt32]);
    // valueAtIndex:maxLength
    XCTAssertEqual(24, [[array valueAtIndex:maxLength] toInt32]);
    // valueAtIndex:maxLength + 1
    XCTAssertEqual(24, [[array valueAtIndex:(maxLength + 1)] toInt32]);
}

- (void)test_Object {
    JSValue *object = [JSValue valueWithNewObjectInContext:context];
    
    object[@"point"] = @{ @"x":@1, @"y":@2 };
    object[@"point"][@"x"] = @3;
    CGPoint point = [object[@"point"] toPoint];
    // toPoint
    XCTAssertEqualWithAccuracy(3, point.x, FLT_EPSILON);
    XCTAssertEqualWithAccuracy(2, point.y, FLT_EPSILON);
    
    object[@{ @"toString":^{ return @"foo"; } }] = @"bar";
    // toString in object literal used as subscript
    XCTAssertEqualObjects(@"bar", [object[@"foo"] toString]);
    
    object[[@"foobar" substringToIndex:3]] = @"bar";
    // substring used as subscript
    XCTAssertEqualObjects(@"bar", [object[@"foo"] toString]);
}

- (void)test_TextXYZ_1 {
    TextXYZ *testXYZ = [[TextXYZ alloc] init];
    context[@"testXYZ"] = testXYZ;
    testXYZ.x = 3;
    testXYZ.y = 4;
    testXYZ.z = 5;
    [context evaluateScript:@"testXYZ.x = 13; testXYZ.y = 14;"];
    [context evaluateScript:@"testXYZ.test('test')"];
    XCTAssertTrue(testXYZ.tested);
    JSValue *result = [context evaluateScript:@"testXYZ.x + ',' + testXYZ.y + ',' + testXYZ.z"];
    XCTAssertEqualObjects(@"13,4,undefined", [result toString]);
}

- (void)test_getterProperty {
    [context[@"Object"][@"prototype"] defineProperty:@"getterProperty" descriptor:@{
                                                                                    JSPropertyDescriptorGetKey:^{
        return [JSContext currentThis][@"x"];
    }
                                                                                    }];
    JSValue *object = [JSValue valueWithObject:@{ @"x":@101 } inContext:context];
    int result = [object [@"getterProperty"] toInt32];
    XCTAssertEqual(101, result);
}

- (void)test_concatenate {
    context[@"concatenate"] = ^{
        NSArray *arguments = [JSContext currentArguments];
        if (![arguments count]) {
            return @"";
        }
        NSString *message = [arguments[0] description];
        for (NSUInteger index = 1; index < [arguments count]; ++index) {
            message = [NSString stringWithFormat:@"%@ %@", message, arguments[index]];
        }
        return message;
    };
    JSValue *result = [context evaluateScript:@"concatenate('Hello,', 'World!')"];
    XCTAssertEqualObjects(@"Hello, World!", [result toString]);
}

- (void)test_boolean {
    context[@"foo"] = @YES;
    XCTAssertTrue([context[@"foo"] isBoolean]);
    JSValue *result = [context evaluateScript:@"typeof foo"];
    XCTAssertEqualObjects(@"boolean", [result toString]);
}

- (void)test_TestObject {
    TestObject* testObject = [TestObject testObject];
    context[@"testObject"] = testObject;
    JSValue *result = [context evaluateScript:@"String(testObject)"];
    XCTAssertEqualObjects(@"[object TestObject]", [result toString]);
}

- (void)test_TestObjectPrototype {
    TestObject* testObject = [TestObject testObject];
    context[@"testObject"] = testObject;
    JSValue *result = [context evaluateScript:@"String(testObject.__proto__)"];
    XCTAssertEqualObjects(@"[object TestObjectPrototype]", [result toString]);
}

- (void)test_TestObjectConstructor {
    context[@"TestObject"] = [TestObject class];
    JSValue *result = [context evaluateScript:@"String(TestObject)"];
    XCTAssertEqualObjects(@"[object TestObjectConstructor]", [result toString]);
}

- (void)test_TestObject_class {
    JSValue* value = [JSValue valueWithObject:[TestObject class] inContext:context];
    XCTAssertEqualObjects([TestObject class], [value toObject]);
}

- (void)test_TestObject_parentTest {
    context[@"TestObject"] = [TestObject class];
    JSValue *result = [context evaluateScript:@"TestObject.parentTest()"];
    XCTAssertEqualObjects(@"TestObject", [result toString]);
}

- (void)test_TestObject_equality {
    TestObject* testObject = [TestObject testObject];
    context[@"testObjectA"] = testObject;
    context[@"testObjectB"] = testObject;
    JSValue *result = [context evaluateScript:@"testObjectA == testObjectB"];
    XCTAssertTrue([result isBoolean]);
    XCTAssertTrue([result toBool]);
}

- (void)test_TestObject_point {
    TestObject* testObject = [TestObject testObject];
    context[@"testObject"] = testObject;
    testObject.point = (CGPoint){3,4};
    JSValue *result = [context evaluateScript:@"var result = JSON.stringify(testObject.point); testObject.point = {x:12,y:14}; result"];
    XCTAssertEqualObjects(@"{\"x\":3,\"y\":4}", [result toString]);
    XCTAssertEqualWithAccuracy(12, testObject.point.x, FLT_EPSILON);
    XCTAssertEqualWithAccuracy(14, testObject.point.y, FLT_EPSILON);
}

- (void)test_TestObject_block {
    TestObject* testObject = [TestObject testObject];
    testObject.six = 6;
    context[@"testObject"] = testObject;
    context[@"mul"] = ^(int x, int y){ return x * y; };
    JSValue *result = [context evaluateScript:@"mul(testObject.six, 7)"];
    XCTAssertTrue([result isNumber]);
    XCTAssertEqual(42, [result toInt32]);
}

- (void)test_TestObject_attaching_pure_js_property {
    TestObject* testObject = [TestObject testObject];
    context[@"testObject"] = testObject;
    context[@"testObject"][@"variable"] = @4;
    [context evaluateScript:@"++testObject.variable"];
    XCTAssertEqual(5, testObject.variable);
}

- (void)test_point {
    context[@"point"] = @{ @"x":@6, @"y":@7 };
    JSValue *result = [context evaluateScript:@"point.x + ',' + point.y"];
    XCTAssertEqualObjects(@"6,7", [result toString]);
}

- (void)test_TestObject_getString {
    TestObject* testObject = [TestObject testObject];
    context[@"testObject"] = testObject;
    JSValue *result = [context evaluateScript:@"testObject.getString()"];
    XCTAssertTrue([result isString]);
    XCTAssertEqual(42, [result toInt32]);
}

- (void)test_TestObject_testArgumentTypes {
    TestObject* testObject = [TestObject testObject];
    context[@"testObject"] = testObject;
    JSValue *result = [context evaluateScript:@"testObject.testArgumentTypes(101,0.5,true,'foo',666,[false,'bar',false],{x:'baz'})"];
    XCTAssertEqualObjects(@"101,0.5,1,foo,666,bar,baz", [result toString]);
}

- (void)test_TestObject_getString_call {
    TestObject* testObject = [TestObject testObject];
    context[@"testObject"] = testObject;
    JSValue *result = [context evaluateScript:@"testObject.getString.call(testObject)"];
    XCTAssertTrue([result isString]);
    XCTAssertEqual(42, [result toInt32]);
}

- (void)test_TestObject_getString_call_exception {
    TestObject* testObject = [TestObject testObject];
    context[@"testObject"] = testObject;
    XCTAssertFalse(context.exception);
    [context evaluateScript:@"testObject.getString.call({})"];
    XCTAssertTrue(context.exception);
}

- (void)test_TestObject_callback {
    TestObject* testObject = [TestObject testObject];
    context[@"testObject"] = testObject;
    JSValue *result = [context evaluateScript:@"var result = 0; testObject.callback(function(x){ result = x; }); result"];
    XCTAssertTrue([result isNumber]);
    XCTAssertEqual(42, [result toInt32]);
    result = [context evaluateScript:@"testObject.bogusCallback"];
    XCTAssertTrue([result isUndefined]);
}

- (void)test_TestObject_Function_prototype_toString_call {
    TestObject *testObject = [TestObject testObject];
    context[@"testObject"] = testObject;
    JSValue *result = [context evaluateScript:@"Function.prototype.toString.call(testObject.callback)"];
    XCTAssertFalse(context.exception);
    XCTAssertFalse([result isUndefined]);
}

- (void)test_passValueBetweenContexts {
    JSContext *context1 = [[JSContext alloc] init];
    JSContext *context2 = [[JSContext alloc] initWithVirtualMachine:context1.virtualMachine];
    JSValue *value = [JSValue valueWithDouble:42 inContext:context2];
    context1[@"passValueBetweenContexts"] = value;
    JSValue *result = [context1 evaluateScript:@"passValueBetweenContexts"];
    XCTAssertTrue([value isEqualToObject:result]);
}

- (void)test_handleTheDictionary {
    __block bool dictionariesAreEqual = false;
    context[@"handleTheDictionary"] = ^(NSDictionary *dict) {
        NSDictionary *expectedDict = @{
                                       @"foo" : [NSNumber numberWithInt:1],
                                       @"bar" : @{
                                               @"baz": [NSNumber numberWithInt:2]
                                               }
                                       };
        // recursively convert nested dictionaries
        dictionariesAreEqual = [dict isEqualToDictionary:expectedDict];
    };
    
    [context evaluateScript:@"var myDict = { \
     'foo': 1, \
     'bar': {'baz': 2} \
     }; \
     handleTheDictionary(myDict);"];
    XCTAssertTrue(dictionariesAreEqual);
    
    __block bool arraysAreEqual = false;
    context[@"handleTheArray"] = ^(NSArray *array) {
        NSArray *expectedArray = @[@"foo", @"bar", @[@"baz"]];
        // recursively convert nested arrays
        arraysAreEqual = [array isEqualToArray:expectedArray];
    };
    [context evaluateScript:@"var myArray = ['foo', 'bar', ['baz']]; handleTheArray(myArray);"];
    XCTAssertTrue(arraysAreEqual);
}

- (void)test_TestObject_Object_getPrototypeOf {
    TestObject *testObject = [TestObject testObject];
    @autoreleasepool {
        context[@"testObject"] = testObject;
        [context evaluateScript:@"var constructor = Object.getPrototypeOf(testObject).constructor; constructor.prototype = undefined;"];
        [context evaluateScript:@"testObject = undefined"];
    }
    
    JSSynchronousGarbageCollectForDebugging([context JSGlobalContextRef]);
    
    @autoreleasepool {
        context[@"testObject"] = testObject;
    }
}

- (void)test_TextXYZ_onclick_weakOnclick {
    TextXYZ *testXYZ = [[TextXYZ alloc] init];
    
    @autoreleasepool {
        context[@"testXYZ"] = testXYZ;
        
        [context evaluateScript:@" \
         didClick = false; \
         testXYZ.onclick = function() { \
         didClick = true; \
         }; \
         \
         testXYZ.weakOnclick = function() { \
         return 'foo'; \
         }; \
         "];
    }
    
    @autoreleasepool {
        [testXYZ click];
        JSValue *result = [context evaluateScript:@"didClick"];
        // Event handler onclick
        XCTAssertTrue([result toBool]);
    }
    
    JSSynchronousGarbageCollectForDebugging([context JSGlobalContextRef]);
    
    @autoreleasepool {
        JSValue *result = [context evaluateScript:@"testXYZ.onclick"];
        // onclick still around after GC
        XCTAssertFalse(([result isNull] || [result isUndefined]));
    }
    
    @autoreleasepool {
        JSValue *result = [context evaluateScript:@"testXYZ.weakOnclick"];
        // weakOnclick not around after GC
        XCTAssertTrue(([result isNull] || [result isUndefined]));
    }
    
    @autoreleasepool {
        [context evaluateScript:@" \
         didClick = false; \
         testXYZ = null; \
         "];
    }
    
    JSSynchronousGarbageCollectForDebugging([context JSGlobalContextRef]);
    
    @autoreleasepool {
        [testXYZ click];
        JSValue *result = [context evaluateScript:@"didClick"];
        // Event handler onclick doesn't fire
        XCTAssertFalse([result toBool]);
    }
}

- (void)test_TestObject_weakValue {
    JSVirtualMachine *vm = [[JSVirtualMachine alloc] init];
    TestObject *testObject = [TestObject testObject];
    JSManagedValue *weakValue;
    // Declaring, allocating and initializing JSVirtualMachine here crashes JavaScriptCore.
    //    JSVirtualMachine *vm = [[JSVirtualMachine alloc] init];

    @autoreleasepool {
        JSContext *context2 = [[JSContext alloc] initWithVirtualMachine:vm];
        context2[@"testObject"] = testObject;
        weakValue = [[JSManagedValue alloc] initWithValue:context2[@"testObject"]];
    }
    
    @autoreleasepool {
        JSContext *context2 = [[JSContext alloc] initWithVirtualMachine:vm];
        context2[@"testObject"] = testObject;
        JSSynchronousGarbageCollectForDebugging([context2 JSGlobalContextRef]);
        // weak value == nil
        XCTAssertFalse([weakValue value]);
        // root is still alive
        XCTAssertFalse([context2[@"testObject"] isUndefined]);
    }
}

- (void)test_TinyDOMNode_1 {
    TinyDOMNode *root = [[TinyDOMNode alloc] init];
    TinyDOMNode *lastNode = root;
    for (NSUInteger i = 0; i < 3; i++) {
        TinyDOMNode *newNode = [[TinyDOMNode alloc] init];
        [lastNode appendChild:newNode];
        lastNode = newNode;
    }
    
    @autoreleasepool {
        root.context[@"root"] = root;
        root.context[@"getLastNodeInChain"] = ^(TinyDOMNode *head){
            TinyDOMNode *lastNode = nil;
            while (head) {
                lastNode = head;
                head = [lastNode childAtIndex:0];
            }
            return lastNode;
        };
        [root.context evaluateScript:@"getLastNodeInChain(root).myCustomProperty = 42;"];
    }
    
    JSSynchronousGarbageCollectForDebugging([root.context JSGlobalContextRef]);
    
    JSValue *myCustomProperty = [root.context evaluateScript:@"getLastNodeInChain(root).myCustomProperty"];
    // My custom property == 42
    XCTAssertTrue([myCustomProperty isNumber]);
    XCTAssertEqual(42, [myCustomProperty toInt32]);
}

- (void)test_TinyDOMNode_2 {
    TinyDOMNode *root = [[TinyDOMNode alloc] init];
    TinyDOMNode *lastNode = root;
    for (NSUInteger i = 0; i < 3; i++) {
        TinyDOMNode *newNode = [[TinyDOMNode alloc] init];
        [lastNode appendChild:newNode];
        lastNode = newNode;
    }
    
    @autoreleasepool {
        root.context[@"root"] = root;
        root.context[@"getLastNodeInChain"] = ^(TinyDOMNode *head){
            TinyDOMNode *lastNode = nil;
            while (head) {
                lastNode = head;
                head = [lastNode childAtIndex:0];
            }
            return lastNode;
        };
        [root.context evaluateScript:@"getLastNodeInChain(root).myCustomProperty = 42;"];
        
        // This is what's different between test_TinyDOMNode_1 and test_TinyDOMNode_2.
        [root appendChild:[root childAtIndex:0]];
        [root removeChildAtIndex:0];
    }
    
    JSSynchronousGarbageCollectForDebugging([root.context JSGlobalContextRef]);
    
    JSValue *myCustomProperty = [root.context evaluateScript:@"getLastNodeInChain(root).myCustomProperty"];
    // Duplicate calls to addManagedReference don't cause things to die.
    XCTAssertTrue([myCustomProperty isNumber]);
    XCTAssertEqual(42, [myCustomProperty toInt32]);
}

- (void)test_JSValue_1 {
    JSValue *object = [JSValue valueWithNewObjectInContext:context];
    object[@"foo"] = @"foo";
    
    JSSynchronousGarbageCollectForDebugging([context JSGlobalContextRef]);
    
    // JSValue correctly protected its internal value
    XCTAssertEqualObjects(@"foo", [object[@"foo"] toString]);
}

- (void)test_TestObject_lookupGetter {
    context[@"testObject"] = [TestObject testObject];
    [context evaluateScript:@"testObject.__lookupGetter__('variable').call({})"];
    // Make sure we throw an exception when calling getter on incorrect |this|"
    XCTAssertTrue(context.exception);
}

- (void)test_TestObject_JSManagedValue {
    TestObject *testObject = [TestObject testObject];
    context[@"testObject"] = testObject;
    JSManagedValue *managedTestObject = [JSManagedValue managedValueWithValue:context[@"testObject"]];
    [context.virtualMachine addManagedReference:managedTestObject withOwner:testObject];
}

@end
