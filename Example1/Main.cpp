#include <worm/EventChannel.h>
#include <worm/EventHandler.h>

#include <chrono>
#include <iostream>
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

class Logger {
public:
    void operator()(const LogEvent& logItem) {
        std::cout << "[Logger] " << logItem.message << std::endl;
    }

private:
    worm::EventHandler<Logger, LogEvent> m_handler{ *this };
};

class NetworkLogger {
public:
    void operator()(const LogEvent& logItem) {
        std::cout << "[NetworkLogger] " << logItem.message << std::endl;
    }

private:
    worm::EventHandler<NetworkLogger, LogEvent> m_handler{ *this };
};

class System {
public:
    void Init() {
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System initialized" }, worm::DispatchType::SYNC);
    }

    void Update() {
        m_counter++;
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System updated - " + std::to_string(m_counter) }, worm::DispatchType::SYNC);
    }

    void Shutdown() {
        worm::EventChannel::Post(LogEvent{ Severity::INFO, "System shut down" }, worm::DispatchType::SYNC);
    }

private:
    uint32_t m_counter{ 0 };
};

int main() {
    Logger logger;
    NetworkLogger networkLogger;
    System system;

    system.Init();

    for (uint32_t i = 0; i < 50; ++i) {
        system.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    system.Shutdown();

    return 0;
}