#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>

class IPAddress {
private:
	unsigned short address[4];
	unsigned short mask[4];
    int maskLength;

public:
    IPAddress(std::string fullAddressText) {
        std::stringstream addressAndMaskStream(fullAddressText);
        std::string IPAndMask[2];
        std::string part;

        size_t i = 0;
        while (std::getline(addressAndMaskStream, part, '/')) {
            IPAndMask[i] = part;
            ++i;
        }
        i = 0;

		std::stringstream IP(IPAndMask[0]);

		while (std::getline(IP, part, '.')) {
            //unsigned long result = std::stoul(str, &pos);
            //todo change to stoi
			address[i] = static_cast<unsigned short>(atoi(part.c_str()));
            ++i;
		}
        maskLength = std::stoi(IPAndMask[1]);
	}

    std::string string() {
        char buffer[20]; 
        sprintf_s(buffer, "%hu.%hu.%hu.%hu/%d", address[0], address[1], address[2], address[3], maskLength);
        std::string str(buffer);
        return str;
    }
};

