#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "IPAddress.h"
#include "Time.h"

class TableEntry {
private:
	std::string source_ = "";
	IPAddress* prefix_;
	std::string metric_ = "";
	std::string nextHop_ = "";
	Time* time_;

public:
	TableEntry(const std::vector<std::string>& args) {
		source_ = args[0];
		prefix_ = new IPAddress(args[1]);
		metric_ = args[2];
		nextHop_ = args[3];
		time_ = new Time(args[4]);
	}

	~TableEntry() {
		//std::cout << "calling destructor\n";
		delete prefix_;
		delete time_;
	}

	IPAddress* getAddress() {
		return prefix_;
	}

	Time* getTime() {
		return time_;
	}

	void print() {
		std::printf("%3s %20s %10s %25s %10s\n", source_.c_str(), prefix_->string().c_str(), metric_.c_str(), nextHop_.c_str(), time_->getFormattedTime().c_str());
	}
};

