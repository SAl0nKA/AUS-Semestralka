//#include <iostream>
#include <vector>
#include "RTParser.h"
#include "Algorithm.h"

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

	std::vector<TableEntry> matches;

	IPAddress* matchAddress = new IPAddress("1.0.0.1");
	auto matchWithAddressPredicate = [&matchAddress](TableEntry otherEntry) {
		int maskTmp = otherEntry.getAddress()->getMaskLength();
		for (size_t i = 0; i < 4; i++) {
			if (otherEntry.getAddress()->getAddressOctet(i) != (matchAddress->getAddressOctet(i) & IPAddress::createMaskNumber(maskTmp > 8 ? 8 : maskTmp))) {
				return false;
			}
			if (maskTmp >= 8) {
				maskTmp -= 8;
			}
		}
		return true;
		};

	Algorithm::matchAddressBy(rt.begin(), rt.end(), matches, 
		[](std::vector<TableEntry>& structure, TableEntry& entry) {
			structure.push_back(entry);
		}, matchWithAddressPredicate);

	//std::printf("Found matches\n");
	std::cout << "Found matches\n";
	for (auto entry : matches) {
		entry.print();
	}

	/*for (const auto& entry : rt) {
		delete entry;
	}*/
}
