#include "CommandProcessor.h"
#include "../Utills/warzoneutils.h"
#include <filesystem>

namespace fs = filesystem;
using warzoneutils::splitInput;
typedef CommandProcessor::CommandType CT;
typedef GameEngine::GameState GS;
// Command
Command::Command(string str) {
	this->command = str;
}

void Command::saveEffect(string str) {
	this->effect = str;
	notify(this);
}

std::string Command::stringToLog() {
	return "Command's Effect:" + this->effect;
}

Command& Command::operator=(const Command& cmd)
{
	this->command = cmd.command;
	this->effect = cmd.effect;
	return *this;
}

std::ostream& operator<<(std::ostream& out, const Command& cmd)
{
	out << "Command: " << cmd.command << " Effect: " << cmd.effect << endl;
	return out;
}

// Command Processor
const unordered_map<string, CT> CommandProcessor::commandmap = { {"loadmap" , CT::LOADMAP }, {"validatemap", CT::VALIDATEMAP}, {"addplayer", CT::ADDPLAYER}, {"gamestart", CT::GAMESTART},
	{"replay", CT::REPLAY}, {"quit", CT::QUIT} };

string CommandProcessor::readCommand() {
	string command;
	getline(cin, command);
	return command;
}

Command* CommandProcessor::getCommand() {
	string str = readCommand();
	Command* command = new Command(str);
	saveCommand(command);
	return command;
}

void CommandProcessor::saveCommand(Command* command) {
	commands.push_back(command);
	for (auto observer : *_observers)
		command->attach(observer);
	notify(this);
}

// Command exists. Checks in the command map if the input string matches a valid command.
bool CommandProcessor::commandexists(string str) {
	return commandmap.count(str) == 1;
}
// Returns a command type to the corresponding input. Assumes the command exists
CT CommandProcessor::getCommandType(string command) {
	vector<string> split;
	splitInput(command, split);

	string maincommand = split[0];
	auto pair = commandmap.find(maincommand);

	return pair->second;
}

bool CommandProcessor::validate(Command* command) {

	// Split up the input command
	vector<string> split;
	splitInput(command->command, split);

	// If it's "", the command is invalid
	if (split.size() == 0) {
		return false;
	}

	if (split.size() > 2) {
		cout << "This command contains too many arguments, please try again." << endl;
		return false;
	}

	string maincommand = split[0];

	if (!commandexists(maincommand)) {
		cout << "This command is not recognized, please try again." << endl;
		return false;
	}

	CT type = getCommandType(command->command);
	GameEngine::GameState gamestate = game->getState();

	bool validity = false;

	switch (type) {
	case CT::LOADMAP: {
		if (gamestate == GS::START || gamestate == GS::MAP_LOADED) {

			if (split.size() < 2) {
				cout << "Missing filename." << endl;
				break;
			}

			string path = "MapFiles/";
			string filename = split[1];

			// Checks if the input filename exists in the correct directory, if it does, then it is valid.
			for (const auto& file : fs::directory_iterator(path)) {
				fs::path map(file.path());

				if (map.filename() == filename) {
					validity = true;
					break;
				}
			}

			if (!validity) {
				cout << "No such map was found in the directory \"" << path << "\"." << endl;
			}
		}
		break;
	}
	case CT::VALIDATEMAP:
		if (gamestate == GS::MAP_LOADED) {
			validity = true;
		}
		break;
	case CT::ADDPLAYER:
		if (gamestate == GS::MAP_VALIDATED || gamestate == GS::PLAYERS_ADDED) {

			if (split.size() < 2) {
				cout << "Missing playername." << endl;
			}
			else if (game->getPlayerCount() == 6) {
				cout << "There is already the max amount of players (6)." << endl;
			}
			else {
				validity = true;
			}
		}
		break;
	case CT::GAMESTART:
		if (gamestate == GS::PLAYERS_ADDED) {
			if (game->getPlayerCount() < 2) {
				cout << "Cannot start the game with less than 2 players." << endl;
			}
			else {
				validity = true;
			}
			
		}
		break;
	case CT::REPLAY:
	case CT::QUIT:
		if (gamestate == GS::WIN) {
			validity = true;
		}
		break;
	}

	if (validity) {
		cout << "Valid command" << endl;
	}
	else {
		cout << "Invalid command" << endl;
		command->saveEffect("Invalid command");
	}

	return validity;

}


CommandProcessor::CommandProcessor()
{
	this->game = nullptr;
}

CommandProcessor::CommandProcessor(GameEngine* game)
{
	this->game = game;
}

CommandProcessor::~CommandProcessor() {
	cout << "Deleting saved commands of CommandProcessor" << endl;
	for (Command* command : commands)
		delete command;
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& cmdProc)
{
	this->commands = cmdProc.commands;
	this->game = cmdProc.game;
	return *this;
}

std::ostream& operator<<(std::ostream& out, const CommandProcessor& cmdProc)
{
	out << "CommandProcessor Commands: " << endl;
	for (Command* cmd : cmdProc.commands)
		out << cmd;
	return out;
}

// FileCommandProcessor

FileCommandProcessorAdapter::FileCommandProcessorAdapter(GameEngine* game, string filename) : CommandProcessor(game) {
	inputstream.open("CommandFiles/" + filename);

}
// If it's the end of the stream, forcefully stop the engine.
string FileCommandProcessorAdapter::readCommand() {
	if (inputstream.eof()) {
		this->game->running = false;
		return "";
	}
	string command;
	getline(inputstream, command);
	if (command == "") {
		this->game->running = false;
		return "";
	}
	return command;
}

std::string CommandProcessor::stringToLog() {
	return "Command: " + commands.back()->command;
}


FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {

	cout << "Closing the file input stream for FileCommandProcessorAdapter." << endl;
	inputstream.close();
}

FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& fCmdProc)
{
	// 'ifstream' has a deleted copy constructor. Nothing to do here.	
	return *this;
}

std::ostream& operator<<(std::ostream& out, const FileCommandProcessorAdapter& fCmdProc)
{
	out << "FileCommandProcessorAdapter Instance" << endl;
	return out;
}
