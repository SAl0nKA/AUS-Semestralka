//#include <iostream>
#include <vector>
#include "RTParser.h"
#include "Algorithm.h"
#include "MemLeak.h"

int main(){
    RTParser rtparser("RT.csv");
    std::vector<TableEntry*> rt;

	int i = 1;
	while (rtparser.isOpen()) {
		TableEntry* entry = rtparser.parseLine();
		if (entry == nullptr) {
			continue;
		}
		/*std::cout << i << " ";
		entry->print();*/
		rt.push_back(entry);
		++i;
	}
	   
	std::vector<TableEntry*> matchesAddress;
	std::vector<TableEntry*> matchesLifetime;

	IPAddress* matchAddress = new IPAddress("1.0.0.1");
	Time* matchLifetimeFrom = new Time(60 * 60 );
	Time* matchLifetimeTo = new Time(60 * 60 * 24);

	auto matchWithAddressPredicate = [&matchAddress](TableEntry* otherEntry) {
		int maskTmp = otherEntry->getAddress()->getMaskLength();
		for (size_t i = 0; i < 4; i++) {
			if (otherEntry->getAddress()->getAddressOctet(i) != (matchAddress->getAddressOctet(i) & IPAddress::createMaskNumber(maskTmp > 8 ? 8 : maskTmp))) {
				return false;
			}
			if (maskTmp >= 8) {
				maskTmp -= 8;
			}
		}
		return true;
		};

	auto matchLifetimePredicate = [&matchLifetimeFrom,&matchLifetimeTo](TableEntry* otherEntry) {
		if (*(otherEntry->getTime()) >= *matchLifetimeFrom && *(otherEntry->getTime()) <= *matchLifetimeTo) {
			return true;
		}
		return false;
		};

	Algorithm::matchAddressBy(rt.begin(), rt.end(),
		[&](TableEntry* entry) {
			matchesAddress.push_back(entry);
		}, matchWithAddressPredicate);

	Algorithm::matchAddressBy(rt.begin(), rt.end(),
		[&](TableEntry* entry) {
			matchesLifetime.push_back(entry);
		}, matchLifetimePredicate);

	std::cout << "Found address matches\n";
	for (auto& entry : matchesAddress) {
		entry->print();
	}

	std::cout << "Found lifetime matches\n";
	for (auto& entry : matchesLifetime) {
		entry->print();
	}

	for (auto entry : rt) {
		delete entry;
	}
	delete matchAddress;
	delete matchLifetimeFrom;
	delete matchLifetimeTo;
}
