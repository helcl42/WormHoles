# WormHoles

WormHoles is a multiplatform header-only library implementing a general and threadsafe event bus.

### Overview
This project is about dispatching messages/events through a system and between its components. An event can be any copyable C++ object.
Currently, it supports three dispatch options. The library's core is based only on two constructs - broadcasting and handling. The former allows you dispatch an event by calling of `worm::EventChannel::Broadcast(<AN_EVENT>, <DISPATCH_TYPE>);` the latter one is all about handling broadcasted events by adding following member `worm::EventHandler< <HANDLER_REFERENCE_TYPE> , <EVENT_TYPE > m_notifyEventsHandler{ <HANDLER_REFERENCE> };` and a handle function `void operator()(const <EVENT_TYPE>& evt)`.

 - `SYNC` - The event is dispatched directly within the current thread(the default one).
 - `ASYNC` - The event is dispatched on another thread from the internal thread pool. It might prevent you from long blocking in event handlers, which is currently a suboptimal solution.
 - `QUEUED` - The event is dispatched when `worm::EventChannel::DispatchAll();` is called. It might be somewhere at the beginning of your main loop. It might prevent you from costly locking. You might not use `QUEUED` way of dispatch so you omit the call `worm::EventChannel::DispatchAll();`

The project has two examples: simple and a bit more complex. For more info, see the sections below.

### Build instructions
- Run `mkdir build && cd build`
- Run `cmake ..`

### TODO
 - ! FIX - make tests !
 - ! OPT - the `ASYNC` way!

## Examples
### Simple Example

This example shows how to create a lousy coupled Logger. No comments touch `Logger` or `NetworkLogger` directly. They post an event to `EventChannel` as the System class does.

```cpp
#include <chrono>
#include <iostream>
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

class Logger {
public:
    void operator()(const LogEvent& logItem)
    {
        std::cout << "Logging message to stdout: " << logItem.message << std::endl;

        // do whatever you want with logItem -> just to sdout here
        // ...
    }

private:
    worm::EventHandler<Logger, LogEvent> m_handler{ *this };
};

class NetworkLogger {
public:
    void operator()(const LogEvent& logItem)
    {
        std::cout << "Logging message to network: " << logItem.message << std::endl;

        // do whatever you want with logItem -> pass it to web-socket if you need
        // ...
    }

private:
    worm::EventHandler<NetworkLogger, LogEvent> m_handler{ *this };
};

class System {
public:
    void Init()
    {
        // ...
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System has been initialized" }, worm::DispatchType::SYNC);
    }

    void Update()
    {
        // ...
        m_counter++;
        // ...
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System has been updated - " + std::to_string(m_counter) }, worm::DispatchType::ASYNC);
    }

    void Shutdown()
    {
        //...
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System has been shut down" }, worm::DispatchType::SYNC);
    }

private:
    uint32_t m_counter{ 0 };
};

int main(int argc, char** argv)
{
    // There is no coupling between System and loggers.
    Logger logger;
    NetworkLogger networkLogger;

    // the System instance depends on EventChannel and LogEvent(sample code only is taken in account)
    System system;

    system.Init();

    for (uint32_t i = 0; i < 50; i++) {
        system.Update();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    system.Shutdown();

    return 0;
}
```

### "Complex" Example

This example shows a bit more advanced usage of `WormHoles` library. There is a main `System` that starts two quite independently running `Subsystem`s. `SubSystem` instances have their working loop threads, and once they make progress with their work, they notify the main `System`, and it generates some `LogEvent` to let the user know about its progress/state.


```cpp
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
public:
    SubSystem(const std::string& name, const std::string& buildingBlock, const uint64_t timeout)
        : m_name{ name }
        , m_buildingBlock{ buildingBlock }
        , m_timeoutInMs{ timeout }
        , m_running{ false }
    {
    }

    ~SubSystem() = default;

public:
    void Init()
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };

        if (m_running) {
            return;
        }

        m_running = true;
        m_thread = std::thread(&SubSystem::Loop, this);

        worm::EventChannel::Post(LogEvent{ Severity::INFO, "SubSystem " + m_name + " has been initialized" }, worm::DispatchType::SYNC);
    }

    void Shutdown()
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };

        if (!m_running) {
            return;
        }

        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }

        worm::EventChannel::Post(LogEvent{ Severity::INFO, "SubSystem " + m_name + " has been shut down" }, worm::DispatchType::SYNC);
    }

    bool IsRunning() const
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };

        return m_running;
    }

private:
    void Loop()
    {
        while (m_running) {
            m_statusString += m_buildingBlock;

            worm::EventChannel::Post(NotifyEvent{ m_name, m_statusString }, m_updateDispatchType);

            std::this_thread::sleep_for(std::chrono::milliseconds(m_timeoutInMs));
        }
    }

private:
    std::string m_name;

    std::string m_buildingBlock;

    uint64_t m_timeoutInMs;

    std::string m_statusString;

    std::thread m_thread;

    mutable std::mutex m_mutex;

    std::atomic<bool> m_running;

    const worm::DispatchType m_updateDispatchType{ worm::DispatchType::QUEUED };
};

class System {
public:
    void Init()
    {
        m_subSystemX.Init();
        m_subSystemY.Init();

        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System has been initialized" }, worm::DispatchType::ASYNC);
    }

    void Update()
    {
        m_counter++;

        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System has been updated - " + std::to_string(m_counter) }, worm::DispatchType::ASYNC);
    }

    void Shutdown()
    {
        m_subSystemY.Shutdown();
        m_subSystemX.Shutdown();

        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System has been shut down" }, worm::DispatchType::ASYNC);
    }

public:
    void operator()(const NotifyEvent& notify)
    {
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System was notified by SubSystem " + notify.subSystemName + " about it's work progress \"" + notify.statusString + "\"" }, worm::DispatchType::SYNC);
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
        worm::EventChannel::DispatchQueued();

        system.Update();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    system.Shutdown();

    return 0;
}