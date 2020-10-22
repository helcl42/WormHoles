# WormHoles

WormHoles is a multiplatform header-only library implementing a general and threadsafe event bus.

### Build status 
| Platform         | Status |
|:----------------:|:------------:|
| Windows          | [![Build status](https://ci.appveyor.com/api/projects/status/qo2sdot4gifga5ux?svg=true)](https://ci.appveyor.com/project/helcl42/wormholes) |
| Linux | [![Linux Build Status](https://travis-ci.org/helcl42/WormHoles.svg?branch=master)](https://travis-ci.org/helcl42/WormHoles "Linux Build Status") |


### Overview
It is all basicaly about dispatching messages/events through a system and between it's components. A event can be any copyable C++ object.
Currently it provides three dispatch options. The usable core of the library is based only on two constructs. The former allows you dispatch an event using calling of `worm::EventChannel::Broadcast(<AN_EVENT>, <DISPATCH_TYPE>);` the latter one is all about handling thoose broadcasted events by adding following  like property `worm::EventHandler< <HANDLER_REFERENCE_TYPE> , <EVENT_TYPE > m_notifyEventsHandler{ <HANDLER_REFERENCE> };` and a handler function `void operator()(const <EVENT_TYPE>& evt)`.

 -  `MAIN_THREAD` - Event is dispatched when `worm::EventChannel::DispatchAll();` is called. It might be somewhere at the beginning of your main loop. It might prevent you from coslty locking everything.
 - `SYNC` - Event is dispatched directly within the current thread.
 - `ASYNC` - Event is dispatched on another thread from internal threadpool. It might prevent you from long blocking in event handlers.

In the project there are two examples simple and a bit more complex. For more see the sections below.

### Build instructions
- Run `mkdir build`
- Run `cd build`
- Run `cmake ..`


## Examples
### Simple Sample

This sample shows how to create a lousy coupled Logger. No compomnents touch `Logger` or `NetworkLogger` directly they just post an event to `EventChannel` like System class does.

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
```

### Complex Sample

This sample shows a bit more advanced usage of `WormHoles` library. There are a main `System` that starts two quite independently running `SubSystem`s. `SubSystem` instances have their own working loop threads and once they make a progress with their work they notify the main `System` and it generates some `LogEvent` to let user about it's progress/state.


```cpp
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
```


## TODO

 - ! make tests !

