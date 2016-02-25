#include <iostream>

#include "WormHoles/EventHandler.h"

struct SomeQuestionEvent
{
	std::string message;

	SomeQuestionEvent(std::string message)
		: message(message) {}

	virtual ~SomeQuestionEvent() {}
};

struct SomeAnswerEvent
{
	std::string message;

	SomeAnswerEvent(std::string message)
		: message(message) {}

	virtual ~SomeAnswerEvent() {}
};

class MagicOracle
{
private:
	WormHoles::EventHandler<MagicOracle, SomeQuestionEvent>* m_handler;

public:
	MagicOracle()
	{
		m_handler = new WormHoles::EventHandler<MagicOracle, SomeQuestionEvent>(this);
	}

	virtual ~MagicOracle() { delete m_handler; }

public:
	void operator()(const SomeQuestionEvent& question)
	{
		std::cout << "I got some question: " << question.message.c_str() << " Unfortunately I've got one generic aswer only." << std::endl;

		WormHoles::EventChannel::Broadcast(SomeAnswerEvent{ "There is no truth!" });
	}
};

class NosyParker
{
private:
	WormHoles::EventHandler<NosyParker, SomeAnswerEvent>* m_handler;

public:
	NosyParker()
	{
		m_handler = new WormHoles::EventHandler<NosyParker, SomeAnswerEvent>(this);
	}

	virtual ~NosyParker() { delete m_handler; }

public:
	void AskQuestion(std::string question)
	{
		WormHoles::EventChannel::Broadcast(SomeQuestionEvent{ question });
	}

public:
	void operator()(const SomeAnswerEvent& question)
	{
		std::cout << "Finally found the answer: " << question.message.c_str() << std::endl;
	}
};

int main(int argc, char** argv)
{
	MagicOracle oracle;

	NosyParker nosyParker;
	nosyParker.AskQuestion("Just tell me, what the truth really is?");

	return 0;
}