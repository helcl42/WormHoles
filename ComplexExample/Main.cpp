#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include <worm/EventChannel.h>
#include <worm/EventHandler.h>

enum class Severity {
    LOG,
    INFO,
    WARNING,
    ERROR
};

// this struct represents an event -> this event might be generated in case you want to log somewhere in your app but you do not want to reference logger
struct LogEvent {
    Severity severity;

    std::string message;
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

struct NotifyEvent {
    std::string subSystemName;

    std::string statusString;
};

class SubSystem {
private:
    void Loop()
    {
        while (m_running) {
            m_statusString += m_buildingBlock;

            worm::EventChannel::Broadcast(NotifyEvent{ m_name, m_statusString }, m_updateDispatchType);

            std::this_thread::sleep_for(std::chrono::milliseconds(m_timeoutInMs));
        }
    }

public:
    SubSystem(const std::string& name, const std::string& buildingBlock, const uint64_t timeout)
        : m_name(name)
        , m_buildingBlock(buildingBlock)
        , m_timeoutInMs(timeout)
        , m_running(false)
    {
    }

    ~SubSystem() = default;

public:
    void Init()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_running) {
            return;
        }

        m_running = true;
        m_thread = std::thread(&SubSystem::Loop, this);

        worm::EventChannel::Broadcast(LogEvent{ Severity::INFO, "SubSystem " + m_name + " has been initialized" }, worm::DispatchType::SYNC);
    }

    void Shutdown()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_running) {
            return;
        }

        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }

        worm::EventChannel::Broadcast(LogEvent{ Severity::INFO, "SubSystem " + m_name + " has been shut down" }, worm::DispatchType::SYNC);
    }

    bool IsRunning() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        return m_running;
    }

private:
    std::string m_name;

    std::string m_buildingBlock;

    uint64_t m_timeoutInMs;

    std::string m_statusString;

    std::thread m_thread;

    mutable std::mutex m_mutex;

    std::atomic<bool> m_running;

    const worm::DispatchType m_updateDispatchType{ worm::DispatchType::ASYNC };
};

class System {
public:
    void Init()
    {
        m_subSystemX.Init();
        m_subSystemY.Init();

        worm::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been initialized" }, worm::DispatchType::SYNC);
    }

    void Update()
    {
        m_counter++;

        worm::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been updated - " + std::to_string(m_counter) }, worm::DispatchType::ASYNC);
    }

    void Shutdown()
    {
        m_subSystemY.Shutdown();
        m_subSystemX.Shutdown();

        worm::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been shut down" }, worm::DispatchType::SYNC);
    }

public:
    void operator()(const NotifyEvent& notify)
    {
        worm::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System was notified by SubSystem " + notify.subSystemName + " about it's work progress \"" + notify.statusString + "\"" }, worm::DispatchType::SYNC);
    }

private:
    worm::EventHandler<System, NotifyEvent> m_notifyEventsHandler{ *this };

private:
    SubSystem m_subSystemX{ "TrippleX", "X", 250 };

    SubSystem m_subSystemY{ "DoubleY", "Y", 375 };

    uint32_t m_counter{ 0 };
};

int main(int argc, char** argv)
{
    // There is no coupling between System, SubSystem and loggers.
    StdOutLogger logger;
    NetworkLogger networkLogger;

    System system;

    system.Init();

    for (uint32_t i = 0; i < 50; i++) {
        worm::EventChannel::DispatchAll();

        system.Update();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    system.Shutdown();

    return 0;
}