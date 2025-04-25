#ifndef __WORM_SINGLETON_TESTS_H__
#define __WORM_SINGLETON_TESTS_H__

#include "../Common.h"

#include <worm/detail/Singleton.h>

#include <thread>

// Example class to test Singleton
class TestSingleton : public worm::detail::Singleton<TestSingleton> {
public:
    void SetValue(int value) { m_value = value; }
    int GetValue() const { return m_value; }

private:
    friend class worm::detail::Singleton<TestSingleton>; // Allow Singleton to access private constructor
    TestSingleton() = default;

    int m_value{ 0 };
};

TEST(SingletonTest, InstanceIsSame)
{
    // Verify that multiple calls to Instance() return the same object
    TestSingleton& instance1 = TestSingleton::Instance();
    TestSingleton& instance2 = TestSingleton::Instance();

    EXPECT_EQ(&instance1, &instance2); // Both should point to the same memory address
}

TEST(SingletonTest, ValuePersistsAcrossCalls)
{
    // Verify that the state persists across calls to Instance()
    TestSingleton& instance = TestSingleton::Instance();
    instance.SetValue(42);

    TestSingleton& sameInstance = TestSingleton::Instance();
    EXPECT_EQ(sameInstance.GetValue(), 42);
}

TEST(SingletonTest, ThreadSafety)
{
    // Verify that Singleton works correctly in a multithreaded environment
    TestSingleton& instance = TestSingleton::Instance();

    std::thread t1([&]() { instance.SetValue(100); });
    std::thread t2([&]() { instance.SetValue(200); });

    t1.join();
    t2.join();

    // The final value depends on the thread execution order, but the instance should remain the same
    EXPECT_EQ(&instance, &TestSingleton::Instance());
}

#endif