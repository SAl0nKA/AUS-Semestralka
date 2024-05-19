#pragma once
#include <vector>
#include "TableEntry.h"
#include "IPAddress.h"
#include <iostream>
#include <limits>
#include "Hierarchy.h"
#include "RTParser.h"
#include "Algorithm.h"

class Console {
private:
	std::vector<TableEntry*> rt_;
	Hierarchy* hierarchy;

public:
	Console(std::string path) {
		printf("Welcome to the routing table parser\n");
		printf("Input the path for the CSV file or press enter for default path [./RT.csv]: ");
		std::string input = "";
		//todo fix incorrect path
		std::getline(std::cin, input);
		if (input == "") {
			loadFile(path);
		} else {
			loadFile(input);
		}
	}

	~Console() {
		for (auto entry : rt_) {
			delete entry;
		}
		delete hierarchy;
	}

private:
	void loadFile(std::string path) {
		RTParser rtparser(path);

		int i = 1;
		while (rtparser.isOpen()) {
			TableEntry* entry = rtparser.parseLine();
			if (entry == nullptr) {
				continue;
			}
			rt_.push_back(entry);
			++i;
		}
		hierarchy = new Hierarchy(rt_);

	}
public:
	bool mainMenu() {
		//printf("Size: %d\n",hierarchy->hierarchy_.size());
		printf("[1] List all available entries\n");
		printf("[2] Find entry by IP\n");
		printf("[3] Find entry by lifetime\n");
		printf("[4] List available first octets\n");
		printf("[5] \n");
		printf("[6] Exit\n");
		printf("Input: ");

		std::string input;
		std::getline(std::cin, input);
		//auto entryTypeDefiner = [](TableEntry* entry) {return entry; };
		switch (input[0]) {
		case '1':
			listAll();
			break;
		case '2':
			//<std::vector<TableEntry*>::iterator, TableEntry*, std::function<TableEntry*(TableEntry*)>>
			findEntryByIP(rt_.begin(), rt_.end());
			break;
		case '3':
			//<std::vector<TableEntry*>::iterator, TableEntry*, std::function<TableEntry* (TableEntry*)>>
			findEntryByLifetime(rt_.begin(), rt_.end());
			break;
		case '4':
			listOctets();
			break;
		case '6':
			return false;
		default:
			printf("Invalid choice, try again\n");
			break;
		}
		return true;
	}

private:
	void listAll() {
		int i = 1;
		for (auto& entry : rt_) {
			std::cout << i << " ";
			entry->print();
			++i;
		}
	}

	template <typename Iterator>
	void findEntryByIP(Iterator begin, Iterator end) {
		printf("Enter your IP address: ");

		std::string inputIP;
		std::getline(std::cin, inputIP);

		if (inputIP.empty()) {
			printf("Invalid choice\n");
			return;
		}

		IPAddress matchAddress;

		try {
			matchAddress = IPAddress(inputIP);
		} catch (const std::logic_error& e) {
			printf("Invalid input: %s\n\n", e.what());
			return;
		}

		ds::amt::ImplicitSequence<TableEntry*> matchedEntries;
		auto inserter = [&](TableEntry* entry) {matchedEntries.insertLast().data_ = entry; };
		auto matchAddressPredicate = [&](TableEntry* otherEntry) {
			return Console::matchWithAddress(matchAddress, otherEntry); };

		Algorithm::matchAddressBy(begin, end,
			inserter, 
			matchAddressPredicate
		);

		
		matchedEntries.isEmpty() ? std::cout << "No addresses found\n" : std::cout << "Found address matches\n";
		int i = 1;
		for (auto entry : matchedEntries) {
			if (entry != nullptr) {
				printf("%d ",i);
				entry->print();
			}
		}
		printf("\n");
	}

	void findNodeByIP(Hierarchy::PreOrderHierarchyIterator begin, Hierarchy::PreOrderHierarchyIterator end) {
		printf("Enter your IP address: ");

		std::string inputIP;
		std::getline(std::cin, inputIP);

		if (inputIP.empty()) {
			printf("Invalid choice\n");
			return;
		}

		ds::amt::ImplicitSequence<TableEntry*> matchedEntries;
		auto inserter = [&](TableEntry* entry) {matchedEntries.insertLast().data_ = entry; };

		IPAddress matchAddress;

		try {
			matchAddress = IPAddress(inputIP);
		} catch (const std::logic_error& e) {
			printf("Invalid input: %s\n\n", e.what());
			return;
		}

		ds::amt::ImplicitSequence<TableEntry*> nodeEntries;
		while (begin != end) {
			for (auto entry : (*begin)->entries_) {
				if (entry == nullptr) {
					continue;
				}
				nodeEntries.insertLast().data_ = entry;
			}
			++begin;
		}

		Algorithm::matchAddressBy(nodeEntries.begin(), nodeEntries.end(),
			inserter,
			[&](TableEntry* otherEntry) {
				return Console::matchWithAddress(matchAddress, otherEntry);
			});

		matchedEntries.isEmpty() ? std::cout << "No addresses found\n" : std::cout << "Found address matches\n";
		int i = 1;
		for (auto entry : matchedEntries) {
			if (entry != nullptr) {
				printf("%d ", i);
				entry->print();
			}
		}
		printf("\n");
	}

	template <class Iterator>
	void findEntryByLifetime(Iterator begin, Iterator end) {
		printf("Enter the first part of the time interval in format [1w2d3h] or [hh:mm:ss]: ");
		std::string inputFrom;
		std::getline(std::cin, inputFrom);

		printf("Enter the second part of the time interval in format [1w2d3h] or [hh:mm:ss]: ");
		std::string inputTo;
		std::getline(std::cin, inputTo);

		if (inputFrom.empty() && inputTo.empty()) {
			printf("Invalid choice\n");
			return;
		}

		Time* matchLifetimeFrom;
		Time* matchLifetimeTo;

		try {
			Time* matchLifetimeFrom = inputFrom.empty() ? new Time(0) : new Time(inputFrom);
			Time* matchLifetimeTo = inputTo.empty() ? new Time(std::numeric_limits<int>::max()) : new Time(inputTo);
		} catch (const std::logic_error& e) {
			printf("Invalid input: %s\n\n", e.what());
			return;
		}

		ds::amt::ImplicitSequence<TableEntry*> matchedEntries;
		auto inserter = [&](TableEntry* entry) {matchedEntries.insertLast().data_ = entry; };

		auto matchLifetimePredicate = [&](TableEntry* otherEntry) {
			return Console::matchLifetime(matchLifetimeFrom, matchLifetimeTo, otherEntry);
		};

		Algorithm::matchAddressBy(begin, end,
			inserter,
			matchLifetimePredicate
		);


		matchedEntries.isEmpty() ? std::cout << "No addresses found\n" : std::cout << "Found address matches\n";
		int i = 1;
		for (auto entry : matchedEntries) {
			if (entry != nullptr) {
				printf("%d ", i);
				entry->print();
			}
		}
		printf("\n");

		delete matchLifetimeFrom;
		delete matchLifetimeTo;
	}

	void findNodeByLifetime(Hierarchy::PreOrderHierarchyIterator begin, Hierarchy::PreOrderHierarchyIterator end) {
		printf("Enter the first part of the time interval in format [1w2d3h] or [hh:mm:ss]: ");
		std::string inputFrom;
		std::getline(std::cin, inputFrom);
		printf("Enter the second part of the time interval in format [1w2d3h] or [hh:mm:ss]: ");
		std::string inputTo;
		std::getline(std::cin, inputTo);

		if (inputFrom.empty() && inputTo.empty()) {
			printf("Invalid choice\n");
			return;
		}

		Time* matchLifetimeFrom;
		Time* matchLifetimeTo;

		try {
			Time* matchLifetimeFrom = inputFrom.empty() ? new Time(0) : new Time(inputFrom);
			Time* matchLifetimeTo = inputTo.empty() ? new Time(std::numeric_limits<int>::max()) : new Time(inputTo);
		} catch (const std::logic_error& e) {
			printf("Invalid input: %s\n\n", e.what());
			return;
		}

		ds::amt::ImplicitSequence<TableEntry*> matchedEntries;
		auto inserter = [&](TableEntry* entry) {matchedEntries.insertLast().data_ = entry; };

		ds::amt::ImplicitSequence<TableEntry*> nodeEntries;
		while (begin != end) {
			for (auto entry : (*begin)->entries_) {
				if (entry == nullptr) {
					continue;
				}
				nodeEntries.insertLast().data_ = entry;
			}
			++begin;
		}

		Algorithm::matchAddressBy(nodeEntries.begin(), nodeEntries.end(),
			inserter,
			[&](TableEntry* otherEntry) {
				return Console::matchLifetime(matchLifetimeFrom, matchLifetimeTo, otherEntry);
			});
			

		matchedEntries.isEmpty() ? std::cout << "No addresses found\n" : std::cout << "Found address matches\n";
		int i = 1;
		for (auto entry : matchedEntries) {
			if (entry != nullptr) {
				printf("%d ", i);
				entry->print();
			}
		}
		printf("\n");

		delete matchLifetimeFrom;
		delete matchLifetimeTo;
	}

	void listOctets() {
		/*hierarchy->visualize(hierarchy->hierarchy_.accessRoot());
		return;*/
		auto iterator = hierarchy->hierarchy_.beginPre();

		int sonsCount = 0;
		
		
		int index;
		int currentLevel = 0;
		int currentOctet = 0;
		
		while (currentLevel != -1) {
			if (currentLevel != 0) {
				printf("\nCurrent octet level: %d\nCurrent octet: %d\n", currentLevel,currentOctet);
			}
			sonsCount = listAvailableOctets(iterator);
			std::string input;
			printf("Select next octet index\nUse -1 to move up a level\nSearch by [l]ifetime\nSearch by [a]ddress\nChoice: ");
			std::getline(std::cin, input);

			if (input.empty()) {
				printf("Invalid choice\n");
				continue;
			}

			//spustenie filtrovania na danom node
			if (input[0] == 'l') {
				filterHierarchy(iterator,'l');
				continue;
			} else if (input[0] == 'a') {
				filterHierarchy(iterator, 'a');
				continue;
			}

			
			try {
				index = std::stoi(input);
			} catch (const std::logic_error& e) {
				printf("Invalid input, try again\n\n");
				continue;
			}

			//vratenie o uroven vyssie
			if (index == -1) {
				currentLevel--;
				if (currentLevel == -1) {
					printf("Exiting\n\n");
					break;
				}
				iterator.goToParent();
				//sonsCount = listAvailableOctets(iterator);
				continue;
			}
			//index out of bounds
			if (index >= sonsCount) {
				printf("Invalid index\n\n");
				continue;
			}

			currentLevel++;
			currentOctet = iterator.goToNthSon(index).getBlockType().data_->octet_;
			//dosiahnutie posledneho syna
			if (currentLevel == 4) {
				auto block = iterator.getBlockType().data_;
				if (block->entries_.size() == 0) {
					printf("There's an empty entry\n");
				} else {
					for (auto entry : block->entries_) {
						entry->print();
					}
				}
				currentLevel--;
				iterator.goToParent();
				sonsCount = listAvailableOctets(iterator);
				continue;
			} 
			//sonsCount = listAvailableOctets(iterator);
		}
	}

	void filterHierarchy(ds::amt::Hierarchy<Hierarchy::HierarchyBlockType>::PreOrderHierarchyIterator iterator,char choice) {
		using Iterator = ds::amt::Hierarchy<ds::amt::MultiWayExplicitHierarchyBlock<Node*>>::PreOrderHierarchyIterator;
		Iterator itBegin(&hierarchy->hierarchy_, &iterator.getBlockType());
		Iterator itEnd(&hierarchy->hierarchy_, nullptr);
		//auto nodeTypeDefiner = [](Node* node) {return node->entries_; };
		if (choice == 'l') {
			//<ds::amt::Hierarchy<Hierarchy::HierarchyBlockType>::PreOrderHierarchyIterator, TableEntry*, std::function<ds::amt::ImplicitSequence<TableEntry*> (Node*)>>
			findNodeByLifetime(itBegin, itEnd);
		} else if (choice == 'a') {
			//<ds::amt::Hierarchy<Hierarchy::HierarchyBlockType>::PreOrderHierarchyIterator, TableEntry*, std::function<ds::amt::ImplicitSequence<TableEntry*> (Node*)>>
			findNodeByIP(itBegin, itEnd);
		}
	}

	int listAvailableOctets(ds::amt::Hierarchy<Hierarchy::HierarchyBlockType>::PreOrderHierarchyIterator& iterator) {
		auto sons = iterator.getBlockType().sons_;
		printf("Available octets: \n");
		size_t i = 0;
		for (auto son : *sons) {
			printf("[%d]-%hu ", i, son->data_->octet_);
			++i;
		}
		printf("\n");
		return sons->size();
	}

	static bool matchWithAddress(IPAddress matchAddress, TableEntry* otherEntry) {
		if (otherEntry == nullptr) {
			return false;
		}
		int maskTmp = otherEntry->getAddress()->getMaskLength();
		for (size_t i = 0; i < 4; i++) {
			if (otherEntry->getAddress()->getAddressOctet(i) != (matchAddress.getAddressOctet(i) & IPAddress::createMaskNumber(maskTmp > 8 ? 8 : maskTmp))) {
				return false;
			}
			if (maskTmp >= 8) {
				maskTmp -= 8;
			}
		}
		return true;
	}

	static bool matchLifetime(Time* matchLifetimeFrom, Time* matchLifetimeTo, TableEntry* otherEntry) {
		if (otherEntry == nullptr) {
			return false;
		}
		if (*(otherEntry->getTime()) >= *matchLifetimeFrom && *(otherEntry->getTime()) <= *matchLifetimeTo) {
			return true;
		}
		return false;
	}

};