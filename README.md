# WormHoles
WormHoles is a multiplatform header-only library implementing an threadsafe Event Bus including features like broadcasting arbitrary events while event handlers have ability to register/unregister from receiving messages(events). 

## Build

- Run `mkdir build`
- Run `cd build`
- Run `cmake ..`

## Usage

This sample show how to create lousy coupled Logger. No Compomnents touch Logger direcly they just post an event to `EventChannel` like System class does. Isn't that nice?

`
#include <iostream>
#include <thread>
#include <chrono>

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
	Logger()
	{
	}

	virtual ~Logger()
	{
	}

public:
	void operator()(const LogEvent& logItem)
	{
		std::cout << "I got some log message: " << logItem.message << std::endl;

		// do whatever you want with logItem
		// ...
	}
};

class System
{
public:
	void Init()
	{
		// ....
		EventChannel::BroadCast({ Severity::INFO, "System has been initialized" });
	}

	void Shutdown()
	{
		//...
		EventChannel::BroadCast({ Severity::INFO, "System has been shut down" });
	}
};

void main(int argc, char** argv)
{
	// There is no coupling between System and Logger.
	Logger logger;

	System system;
	system.Init();

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	system.Shutdown();

	return 0;
}
`
- For another example take a look at Main.cpp.

- This library might be "inlined" in single header for even simpler integration into an existing project.

## TODO
 - add more complex examples

 - make tests

