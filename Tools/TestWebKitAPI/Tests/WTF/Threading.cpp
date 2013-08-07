/*
 */

#include "config.h"
#include <wtf/Threading.h>

namespace TestWebKitAPI {

class ThreadingTest : public testing::Test {
public:
    virtual void SetUp()
    {
        WTF::initializeThreading();
    }

    static void simple_thread_init(void *data)
    {
        bool *thread_started = static_cast<bool*>(data);

        *thread_started = true;
    }
    static void test_current_thread_thread_init(void *data)
    {
        ThreadIdentifier *thread_id = static_cast<ThreadIdentifier *>(data);

        *thread_id = currentThread();
    }
};


TEST_F(ThreadingTest, Simple)
{
    bool thread_started = false;
    ThreadIdentifier test_thread = createThread(simple_thread_init, &thread_started, "test_thread");

    waitForThreadCompletion (test_thread);

    EXPECT_TRUE (thread_started);
}

TEST_F(ThreadingTest, CurrentThread)
{
    ThreadIdentifier main_thread = currentThread();
    ThreadIdentifier test_thread_from_thread;
    ThreadIdentifier test_thread = createThread(test_current_thread_thread_init, &test_thread_from_thread, "test_thread");

    EXPECT_NE (main_thread, test_thread);

    waitForThreadCompletion (test_thread);

    EXPECT_EQ (test_thread_from_thread, test_thread);
}

} // namespace TestWebKitAPI
