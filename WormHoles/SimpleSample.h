#ifndef __SIMPLE_SAMPLE_H__
#define __SIMPLE_SAMPLE_H__

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <WormHoles/EventChannel.h>
#include <WormHoles/EventHandler.h>

namespace SimpleSample {
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
private:
    WormHoles::EventHandler<Logger, LogEvent> m_handler{ *this };

public:
    void operator()(const LogEvent& logItem)
    {
        std::cout << "Logging message to stdout: " << logItem.message << std::endl;

        // do whatever you want with logItem -> just to sdout here
        // ...
    }
};

class NetworkLogger {
private:
    WormHoles::EventHandler<NetworkLogger, LogEvent> m_handler{ *this };

public:
    void operator()(const LogEvent& logItem)
    {
        std::cout << "Logging message to network: " << logItem.message << std::endl;

        // do whatever you want with logItem -> pass it to web-socket if you need
        // ...
    }
};

class System {
private:
    uint32_t m_counter{ 0 };

public:
    void Init()
    {
        // ...
        WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been initialized" }, WormHoles::DispatchType::SYNC);
    }

    void Update()
    {
        // ...
        m_counter++;
        // ...
        WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been updated - " + std::to_string(m_counter) }, WormHoles::DispatchType::ASYNC);
    }

    void Shutdown()
    {
        //...
        WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been shut down" }, WormHoles::DispatchType::SYNC);
    }
};

void Run()
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
}
} // namespace SimpleSample

#endif