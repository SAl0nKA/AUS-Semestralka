#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "IPAddress.h"

class TableEntry {
private:
	std::string source_ = "";
	IPAddress* prefix_;
	std::string metric_ = "";
	std::string nextHop_ = "";
	std::string time_ = "";
public:
	TableEntry(const std::vector<std::string>& args) {
		//if (args.size() != 5) {
			//std::cout << "Wrong arguments\n";
			//return;
		//}

		source_ = args[0];
		prefix_ = new IPAddress(args[1]);
		metric_ = args[2];
		nextHop_ = args[3];
		time_ = args[4];
	}

	~TableEntry() {
		delete prefix_;
	}

	void print() {
		std::printf("%3s %20s %10s %25s %10s\n", source_.c_str(), prefix_->string().c_str(), metric_.c_str(), nextHop_.c_str(), time_.c_str());
	}
};

