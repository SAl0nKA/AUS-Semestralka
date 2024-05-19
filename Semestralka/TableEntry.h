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
	IPAddress* nextHop_;
	Time* time_;

public:
	TableEntry(const std::vector<std::string>& args) {
		source_ = args[0];
		prefix_ = new IPAddress(args[1]);
		metric_ = args[2];
		nextHop_ = new IPAddress(args[3].substr(4, args[3].size()));
		time_ = new Time(args[4]);
	}

	~TableEntry() {
		//std::cout << "calling destructor\n";
		delete prefix_;
		delete time_;
		delete nextHop_;
	}

	IPAddress* getAddress() const {
		return prefix_;
	}

	IPAddress* getNextHop() const {
		return nextHop_;
	}

	Time* getTime() const {
		return time_;
	}

	void print() const {
		std::printf("%3s %20s %10s %25s %10s\n", source_.c_str(), prefix_->string().c_str(), metric_.c_str(), nextHop_->stringNextHop().c_str(), time_->getFormattedTime().c_str());
	}
};

