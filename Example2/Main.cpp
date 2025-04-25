#include <worm/EventChannel.h>
#include <worm/EventHandler.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

enum class Severity {
    LOG,
    INFO,
    WARNING,
    ERROR
};

// Event structure for logging
struct LogEvent {
    Severity severity;
    std::string message;
};

struct NotifyEvent {
    std::string subSystemName;
    std::string statusString;
};

class StdOutLogger final {
public:
    void operator()(const LogEvent& logItem)
    {
        std::cout << "Logging message to stdout: " << logItem.message << " ThreadId: " << std::this_thread::get_id() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        // do whatever you want with logItem -> just to sdout here
        // ...
    }

private:
    worm::EventHandler<StdOutLogger, LogEvent> m_logEventsHandler{ *this };
};

class NetworkLogger final {
public:
    void operator()(const LogEvent& logItem)
    {
        std::cout << "Logging message to network: " << logItem.message << " ThreadId: " << std::this_thread::get_id() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(40));

        // do whatever you want with logItem -> pass it to web-socket if you need
        // ...
    }

private:
    worm::EventHandler<NetworkLogger, LogEvent> m_logEventsHandler{ *this };
};

class SubSystem {
public:
    SubSystem(const std::string& name, const std::string& buildingBlock, uint64_t timeout)
        : m_name(name)
        , m_buildingBlock(buildingBlock)
        , m_timeoutInMs(timeout)
        , m_running(false)
    {
    }

    void Init()
    {
        std::scoped_lock lock(m_mutex);
        if (m_running)
            return;

        m_running = true;
        m_thread = std::thread(&SubSystem::Loop, this);
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "SubSystem " + m_name + " initialized" }, worm::DispatchType::SYNC);
    }

    void Shutdown()
    {
        std::scoped_lock lock(m_mutex);
        if (!m_running)
            return;

        m_running = false;
        if (m_thread.joinable())
            m_thread.join();
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "SubSystem " + m_name + " shut down" }, worm::DispatchType::SYNC);
    }

private:
    void Loop()
    {
        while (m_running) {
            m_statusString += m_buildingBlock;
            worm::EventChannel::Post(NotifyEvent{ m_name, m_statusString }, worm::DispatchType::QUEUED);
            std::this_thread::sleep_for(std::chrono::milliseconds(m_timeoutInMs));
        }
    }

    std::string m_name;
    std::string m_buildingBlock;
    uint64_t m_timeoutInMs;
    std::string m_statusString;
    std::thread m_thread;
    mutable std::mutex m_mutex;
    std::atomic<bool> m_running;
};

class System {
public:
    void Init()
    {
        m_subSystemX.Init();
        m_subSystemY.Init();
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System initialized" }, worm::DispatchType::ASYNC);
    }

    void Update()
    {
        ++m_counter;
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System updated - " + std::to_string(m_counter) }, worm::DispatchType::ASYNC);
    }

    void Shutdown()
    {
        m_subSystemX.Shutdown();
        m_subSystemY.Shutdown();
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System shut down" }, worm::DispatchType::ASYNC);
    }

    void operator()(const NotifyEvent& notify)
    {
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System notified by " + notify.subSystemName + ": " + notify.statusString }, worm::DispatchType::SYNC);
    }

private:
    worm::EventHandler<System, NotifyEvent> m_notifyEventsHandler{ *this };
    SubSystem m_subSystemX{ "SubSystemX", "X", 250 };
    SubSystem m_subSystemY{ "SubSystemY", "Y", 375 };
    uint32_t m_counter{ 0 };
};

int main()
{
    StdOutLogger stdOutLogger;
    NetworkLogger networkLogger;

    System system;

    system.Init();

    for (uint32_t i = 0; i < 50; ++i) {
        worm::EventChannel::DispatchAllQueued();
        system.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    worm::EventChannel::DispatchAll();

    system.Shutdown();

    return 0;
}