#pragma once

#include <fstream>
#include <sstream>
#include "TableEntry.h"
#include <vector>

class RTParser {
private:
	std::ifstream file;

public:
	RTParser(std::string filepath) {
		file.open(filepath);

		//skip first line
		std::string line;
		std::getline(file, line);
	}

	~RTParser() {
		file.close();
		/*if (file != NULL) {

		}*/
	}

	bool isOpen() {
		return file.is_open();
	}

	std::string readLine() {
		if (!this->isOpen()) {
			return "";
		}

		if (file.eof()) {
			file.close();
			return "";
		}

		std::string line;
		std::getline(file, line);
		return line;
	}

	TableEntry* parseLine() {
		std::string line = this->readLine();
		if (line.length() == 0) {
			return nullptr;
		}
		std::stringstream ss(line);

		std::vector<std::string> parts;
		std::string part;
		while (std::getline(ss, part, ';')) {
			parts.push_back(part == "" ? "" : part);
		}
		if (line.back() == ';') {
			parts.push_back(""); 
		}


		return new TableEntry(parts);
	}
};
