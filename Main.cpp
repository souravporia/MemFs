#include "src/FileSystem.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

// Function to split a string into tokens, handling quoted strings as single tokens
std::vector<std::string> split(const std::string& str) {
	std::vector<std::string> tokens;
	std::istringstream stream(str);
	std::string token;
	bool inQuotes = false;
	std::string quotedToken;

	while (stream >> std::quoted(token)) {
		tokens.push_back(token);
	}

	return tokens;
}

// Parse numeric option for commands like `create -n 3 ...`
int parseNumericOption(const std::vector<std::string>& tokens, int defaultValue) {
	for (size_t i = 1; i < tokens.size(); ++i) {
		if (tokens[i] == "-n" && i + 1 < tokens.size()) {
			return std::stoi(tokens[i + 1]);
		}
	}
	return defaultValue;
}

// Command-line handling and testing of FileSystem
int main() {
	VirtualDisk vdisk;
	FileSystem memFS(vdisk);
	std::string command;

	while (true) {
		std::cout << "memfs> ";
		std::getline(std::cin, command);

		std::vector<std::string> tokens = split(command);
		if (tokens.empty()) {
			std::cerr << "Invalid command: Empty command\n";
			continue;
		}

		std::string commandName = tokens[0];

		if (commandName == "create") {
			size_t numFiles = parseNumericOption(tokens, 1);
			for (size_t i = tokens.size() - numFiles; i < tokens.size(); ++i) {
				std::string filename = tokens[i];
				memFS.createFile(filename);
			}
		}
		else if (commandName == "write") {
			if (tokens.size() < 3) {
				std::cerr << "Invalid command: Missing filename or content\n";
				continue;
			}

			int numFiles = parseNumericOption(tokens, 1);
			size_t contentStartIndex = 2 + (numFiles > 1 ? 1 : 0);
			std::string content = tokens[contentStartIndex];

			for (int i = 1; i <= numFiles; ++i) {
				std::string filename = tokens[i];
				std::vector<char> char_vector(content.begin(), content.end());
				memFS.writeFile(filename, char_vector);
			}
		}
		else if (commandName == "delete") {
			size_t numFiles = parseNumericOption(tokens, 1);
			for (size_t i = tokens.size() - numFiles; i < tokens.size(); ++i) {
				std::string filename = tokens[i];
				memFS.deleteFile(filename);
			}
		}
		else if (commandName == "read") {
			if (tokens.size() < 2) {
				std::cerr << "Invalid command: Missing filename\n";
				continue;
			}
			std::string filename = tokens[1];
			std::vector<char> char_vector;
			memFS.read(filename, char_vector);
			std::cout << std::string(char_vector.begin(), char_vector.end()) << std::endl;
		}
		else if (commandName == "ls") {
			bool detailed = tokens.size() > 1 && tokens[1] == "-l";
			memFS.listFiles(detailed ? 1 : 0);
		}
		else {
			std::cerr << "Invalid command: " << commandName << "\n";
		}
	}

	return 0;
}