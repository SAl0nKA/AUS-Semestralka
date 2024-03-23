//#include <iostream>
#include <vector>
#include "RTParser.h"

int main(){
    RTParser rtparser("RT.csv");
    std::vector<TableEntry*> rt;

	int i = 1;
	while (rtparser.isOpen()) {
		TableEntry* entry = rtparser.parseLine();
		if (entry == nullptr) {
			continue;
		}
		std::cout << i << " ";
		entry->print();
		rt.push_back(entry);
		++i;
	}

	for (const auto& entry : rt) {
		delete entry;
	}
}
