#pragma once
#include <string>
#include <libds/heap_monitor.h>

class Time {
private:
	std::string formattedTime_;
	int seconds_ = 0;

public:
    Time(int seconds) {
        seconds_ = seconds;
    }

	Time(std::string formattedTime) {
		formattedTime_ = formattedTime;
		if (formattedTime.find(":") != std::string::npos) {
			int hours, minutes, seconds;
			sscanf_s(formattedTime.c_str(), "%d:%d:%d", &hours, &minutes, &seconds);
			seconds_ = seconds + minutes * MINUTE + hours * HOUR;
			return;
		}

        size_t pos = 0;
        while (pos < formattedTime.length()) {
            size_t nextPos = formattedTime.find_first_not_of("0123456789", pos);
            int value = std::stoi(formattedTime.substr(pos, nextPos - pos));
            char unit = formattedTime[nextPos];

            switch (unit) {
            case 'w':
				seconds_ += value * WEEK;
                break;
            case 'd':
				seconds_ += value * DAY;
                break;
            case 'h':
				seconds_ += value * HOUR;
                break;
            }

            pos = nextPos + 1;
        }
	}

	~Time() {

	}

	std::string getFormattedTime() {
		return formattedTime_;
	}

    int getSeconds() {
        return seconds_;
    }

    bool operator==(const Time& other) const {
        return seconds_ == other.seconds_;
    }

    bool operator>(const Time& other) const {
        return seconds_ > other.seconds_;
    }

    bool operator<(const Time& other) const {
        return seconds_ < other.seconds_;
    }

    bool operator<=(const Time& other) const {
        return seconds_ <= other.seconds_;
    }

    bool operator>=(const Time& other) const {
        return seconds_ >= other.seconds_;
    }

private:
	static const int MINUTE = 60;
	static const int HOUR = 60 * 60;
	static const int DAY = 60 * 60 * 24;
	static const int WEEK = 60 * 60 * 24 * 7;
};
