#pragma once
#include <list>
#include <string>
#include <fstream>
#include "../Game Engine/GameEngine.h"
using namespace std;


class Command : public Subject, public ILoggable {
public:
	string command;
	string effect;

	Command(string);

	void saveEffect(string);
	std::string stringToLog() override;
};
class CommandProcessor : public Subject, public ILoggable {
protected:
	virtual string readCommand();
	void saveCommand(Command*);
public:
	enum class CommandType {
		LOADMAP = 0,
		VALIDATEMAP,
		ADDPLAYER,
		GAMESTART,
		REPLAY,
		QUIT
	};
	list<Command*> commands;
	Command* getCommand();

	~CommandProcessor();
	std::string stringToLog() override;


private:
	static const unordered_map<string, CommandType> commandmap;

};
class FileCommandProcessorAdapter : public CommandProcessor {
public:
	FileCommandProcessorAdapter(string);
protected:
	ifstream inputstream;
	string readCommand();
};