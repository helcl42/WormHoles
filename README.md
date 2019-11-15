# WormHoles
WormHoles is a multiplatform header-only library implementing a threadsafe Event Bus. It includes features like broadcasting arbitrary events and ability to register an instance to receive specific types of events 

## Build 
| Platform         | Status |
|:----------------:|:------------:|
| Windows          | [![Build status](https://ci.appveyor.com/api/projects/status/qo2sdot4gifga5ux?svg=true)](https://ci.appveyor.com/project/helcl42/wormholes) |
| Linux | [![Linux Build Status](https://travis-ci.org/helcl42/WormHoles.svg?branch=master)](https://travis-ci.org/helcl42/WormHoles "Linux Build Status") |

### Instructions
- Run `mkdir build`
- Run `cd build`
- Run `cmake ..`

## Usage

In the project there are two sample simple and complex. The former example shows basic usage and helps you better understad the latter one.

### Simple Sample

This sample show how to create a lousy coupled Logger. No compomnents touch `Logger` or `NetworkLogger` direcly they just post an event to `EventChannel` like System class does.

```cpp
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
	WormHoles::EventHandler<Logger, LogEvent> m_handler{ *this };

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
	WormHoles::EventHandler<NetworkLogger, LogEvent> m_handler{ *this };

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

void Run()
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
}
```

### Complex Sample

This sample shows a bit more advanced usage of `WormHoles` library. There are a main `System` that starts two quite independently running `SubSystem`s. `SubSystem` instances have their own working loop threads and once they make a progress with their work they notify the main `System` and it generates some `LogEvent` to let user about it. There are two options. They might `Broadcast` their `NotifyEvent` events directly in their loop thread context or they might `BroadcastWithDispatch` and postpone delivery up to the moment when the main thread calls `DispatchAll` and delivers events in it's context. This option is controlled by choosing a broadcast method. You can change by changing value of `SYNCRONOUS_DISPATCH` to see what happens. Please note ThreadId in output logs.(they are the same for all(`BroadcastWitDispatch` is used) or not).

#### Dispatch Options

-  `Broadcast`- an event is delivered synchronously in current thread context
 - `BroadcastWithDispatch` - an event is just stored in a queue and it is delivered once a thread calls `DispatchAll()` -> all stored events in the queue are the delivered in the thread context


```cpp
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

#include <WormHoles/EventHandler.h>
#include <WormHoles/EventChannel.h>

constexpr bool SYNCRONOUS_DISPATCH = true;

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

class AbstractLogger
{
private:
	WormHoles::EventHandler<AbstractLogger, LogEvent> m_logEventsHandler{ *this };

public:
	AbstractLogger() = default;

	virtual ~AbstractLogger() = default;

public:
	virtual void operator()(const LogEvent& logItem) = 0;
};

class StdOutLogger final : public AbstractLogger
{
public:
	void operator()(const LogEvent& logItem) override
	{
		std::cout << "Logging message to stdout: " << logItem.message << " ThreadId: " << std::this_thread::get_id() << std::endl;

		// do whatever you want with logItem -> just to sdout here
		// ...
	}
};

class NetworkLogger final : public AbstractLogger
{
public:
	void operator()(const LogEvent& logItem) override
	{
		std::cout << "Logging message to network: " << logItem.message << " ThreadId: " << std::this_thread::get_id() << std::endl;

		// do whatever you want with logItem -> pass it to web-socket if you need
		// ...
	}
};

struct NotifyEvent
{
	std::string subSystemName;

	std::string statusString;
};

class SubSystem
{
private:
	std::string m_name;

	std::string m_buildingBlock;

	long long m_timeoutInMs;

	std::string m_statusString;

	std::thread m_thread;

	mutable std::mutex m_mutex;

	std::atomic<bool> m_running;

private:
	void Loop()
	{
		while (m_running)
		{
			m_statusString += m_buildingBlock;

			NotifyEvent newNotifyEvent{ m_name, m_statusString };

			if (SYNCRONOUS_DISPATCH)
			{
				WormHoles::EventChannel::BroadcastWithDispatch(newNotifyEvent);
			}
			else
			{
				WormHoles::EventChannel::Broadcast(newNotifyEvent);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(m_timeoutInMs));
		}
	}

public:
	SubSystem(const std::string& name, const std::string& buildingBlock, long long timeout)
		: m_name(name), m_buildingBlock(buildingBlock), m_timeoutInMs(timeout), m_running(false)
	{
	}

	~SubSystem() = default;
	
public:
	void Init()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_running)
		{
			return;
		}

		m_running = true;
		m_thread = std::thread(&SubSystem::Loop, this);

		WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "SubSystem " + m_name + " has been initialized" });
	}

	void Shutdown()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (!m_running)
		{
			return;
		}

		m_running = false;
		if (m_thread.joinable())
		{
			m_thread.join();
		}

		WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "SubSystem " + m_name + " has been shut down" });
	}

	bool IsRunning() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_running;
	}
};

class System
{
private:
	WormHoles::EventHandler<System, NotifyEvent> m_notifyEventsHandler{ *this };

private:
	SubSystem m_subSystemX{ "TrippleX", "X", 250 };

	SubSystem m_subSystemY{ "DoubleY", "Y", 375 };

	uint32_t m_counter{ 0 };

public:
	void Init()
	{
		m_subSystemX.Init();
		m_subSystemY.Init();

		WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been initialized" });
	}

	void Update()
	{
		m_counter++;

		WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been updated - " + std::to_string(m_counter) });
	}

	void Shutdown()
	{
		m_subSystemY.Shutdown();
		m_subSystemX.Shutdown();

		WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System has been shut down" });
	}

public:
	void operator() (const NotifyEvent& notify)
	{
		WormHoles::EventChannel::Broadcast(LogEvent{ Severity::INFO, "System was notified by SubSystem " + notify.subSystemName + " about it's work progress \"" + notify.statusString + "\"" });
	}
};

void Run()
{
	// There is no coupling between System, SubSystem and loggers.
	StdOutLogger logger;
	NetworkLogger networkLogger;

	System system;

	system.Init();

	for (uint32_t i = 0; i < 50; i++)
	{
		WormHoles::EventChannel::DispatchAll();

		system.Update();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	system.Shutdown();
}
```


## TODO

 - this library might be "inlined" into a single header (there are four at the moment)

 - make tests

