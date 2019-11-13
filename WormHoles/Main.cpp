#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <WormHoles/EventHandler.h>
#include <WormHoles/EventChannel.h>

enum class Severity
{
	LOG,
	INFO,
	WARNING,
	ERROR
};

// this struct represents an event -> this event might be generated in case you want to log somewhere in your app but you do not want to reference logger
struct LogEvent
{
	Severity severity;

	std::string message;
};

class Logger
{
private:
	WormHoles::EventHandler<Logger, LogEvent> m_handler{ this };

public:
	void operator()(const LogEvent& logItem)
	{
		std::cout << "Logging message to stdout: " << logItem.message << std::endl;

		// do whatever you want with logItem -> just to sdout here
		// ...
	}
};

class NetworkLogger
{
private:
	WormHoles::EventHandler<NetworkLogger, LogEvent> m_handler{ this };

public:
	void operator()(const LogEvent& logItem)
	{
		std::cout << "Logging message to network: " << logItem.message << std::endl;

		// do whatever you want with logItem -> pass it to web-socket if you need
		// ...
	}
};

class System
{
private:
	uint32_t m_counter{ 0 };

public:
	void Init()
	{
		// ...
		WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been initialized" });
	}

	void Update()
	{
		// ...
		m_counter++;
		// ...
		WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been updated - " + std::to_string(m_counter) });
	}
	 
	void Shutdown()
	{
		//...
		WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been shut down" });
	}
};

int main(int argc, char** argv)
{
	// There is no coupling between System and loggers.
	Logger logger;
	NetworkLogger networkLogger;

	// the System instance depends on EventChannel and LogEvent(sample code only is taken in account)
	System system;

	system.Init();

	for (uint32_t i = 0; i < 5; i++)
	{
		system.Update();

		std::this_thread::sleep_for(std::chrono::milliseconds(1200));
	}

	system.Shutdown();

	return 0;
}