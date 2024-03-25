#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>

class IPAddress {
private:
    std::uint8_t address_[4];
	//unsigned short mask_[4];
    int maskLength_ = 0;

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
            address_[i] = static_cast<std::uint8_t>(atoi(part.c_str()));
            ++i;
		}
        
        maskLength_ = std::stoi(IPAndMask[1] == "" ? "0" : IPAndMask[1]);
	}

    ~IPAddress() {

    }

    int getMaskLength() {
        return maskLength_;
    }

    std::uint8_t getAddressOctet(int index) {
        if (index < 0 || index > 3) {
            return -1;
        }
        return address_[index];
    }

    std::string string() {
        char buffer[20]; 
        sprintf_s(buffer, "%hu.%hu.%hu.%hu/%d", address_[0], address_[1], address_[2], address_[3], maskLength_);
        std::string str(buffer);
        return str;
    }

    //static bool matchWithAddress(IPAddress matchAddress,IPAddress otherAddress) {
    //    /*int notFullOctet = matchAddress.maskLength_ % 8;
    //    int fullOctets = (matchAddress.maskLength_ - notFullOctet) / 8;*/
    //    int maskTmp = matchAddress.maskLength_;
    //    for (size_t i = 0; i < 4; i++) {
    //        if ((otherAddress.address_[i] & createMaskNumber(maskTmp > 8 ? 8 : maskTmp)) != matchAddress.address_[i]) {
    //            return false;
    //        }
    //        maskTmp -= 8;
    //    }
    //    return true;
    //}

//private:
    static std::uint8_t createMaskNumber(int bits) {
        std::uint8_t maskNumber = 0;
        std::uint8_t adder = 128;
        for (int i = 0; i < bits; i++) {
            maskNumber += adder;
            adder /= 2;
        }
        //std::cout << "Mask number: " << maskNumber << std::endl;
        return maskNumber;
    }
};

