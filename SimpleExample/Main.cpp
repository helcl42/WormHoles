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
		worm::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been initialized" }, worm::DispatchType::SYNC);
	}

	void Update()
	{
		// ...
		m_counter++;
		// ...
		worm::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been updated - " + std::to_string(m_counter) }, worm::DispatchType::ASYNC);
	}

	void Shutdown()
	{
		//...
		worm::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been shut down" }, worm::DispatchType::SYNC);
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