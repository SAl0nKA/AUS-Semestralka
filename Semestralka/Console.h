#pragma once
#include <vector>
#include "TableEntry.h"
#include "IPAddress.h"
#include <iostream>
#include <limits>
#include "Hierarchy.h"
#include "RTParser.h"
#include "Algorithm.h"
#include "Treap.h"
#include "Sorter.h"
#include <libds/heap_monitor.h>

class Console {
private:
	std::vector<TableEntry*> rt_;
	Hierarchy* hierarchy;
	Treap* treap;

public:
	Console(std::string path) {
		hierarchy = new Hierarchy();
		treap = new Treap();

		printf("Welcome to the routing table parser\n");
		printf("Input the path for the CSV file or press enter for default path [./RT.csv]: ");

		std::string input = "";
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
		delete treap;
	}

private:
	void loadFile(std::string path) {
		RTParser rtparser(path);
		
		//int i = 1;
		while (rtparser.isOpen()) {
			TableEntry* entry = rtparser.parseLine();
			if (entry == nullptr) {
				continue;
			}
			rt_.push_back(entry);
			hierarchy->addToHierarchy(entry);
			treap->addToTreap(entry);
			//++i;
		}
	}

public:
	bool mainMenu() {
		//printf("Size: %d\n",hierarchy->hierarchy_.size());
		printf("[1] List all available entries\n");
		printf("[2] Find entry by IP\n");
		printf("[3] Find entry by lifetime\n");
		printf("[4] List available first octets\n");
		printf("[5] Find by next hop IP\n");
		printf("[6] Exit\n");
		printf("Input: ");

		std::string input;
		std::getline(std::cin, input);

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
		case '5':
			findByNexthop();
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

	//Level 1
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
			return Console::matchWithAddress(matchAddress, otherEntry); 
		};

		Algorithm::matchAddressBy(begin, end,
			inserter, 
			matchAddressPredicate
		);

		printEntries(matchedEntries);
	}

	//Level 2
	void findNodeByIP(Hierarchy::PreOrderHierarchyIterator begin, Hierarchy::PreOrderHierarchyIterator end) {
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
		auto addressPredicate = [&](TableEntry* otherEntry) {
			return Console::matchWithAddress(matchAddress, otherEntry);
		};
		auto seqeunceHandler = [&](Node* node) {
			ds::amt::ImplicitSequence<TableEntry*> is = node->entries_;
			if (is.size() == 0) {
				return false;
			}
			//spustenie predikatu nad IS
			Algorithm::matchAddressBy(is.begin(), is.end(), inserter, addressPredicate);
			};
		//spustenie sequenceHandler nad hierarchiou
		Algorithm::matchAddressBy(begin, end, [](Node* node) {}, seqeunceHandler);

		//Level 4
		sortSequence(matchedEntries);
		printEntries(matchedEntries);
	}

	//Level 1
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
			matchLifetimeFrom = inputFrom.empty() ? new Time(0) : new Time(inputFrom);
			matchLifetimeTo = inputTo.empty() ? new Time(std::numeric_limits<int>::max()) : new Time(inputTo);
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


		printEntries(matchedEntries);

		delete matchLifetimeFrom;
		delete matchLifetimeTo;
	}

	//Level 2
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
			matchLifetimeFrom = inputFrom.empty() ? new Time(0) : new Time(inputFrom);
			matchLifetimeTo = inputTo.empty() ? new Time(std::numeric_limits<int>::max()) : new Time(inputTo);
		} catch (const std::logic_error& e) {
			printf("Invalid input: %s\n\n", e.what());
			return;
		}

		ds::amt::ImplicitSequence<TableEntry*> matchedEntries;
		auto inserter = [&](TableEntry* entry) {matchedEntries.insertLast().data_ = entry; };
		auto lifetimePredicate = [&](TableEntry* otherEntry) {
			return Console::matchLifetime(matchLifetimeFrom, matchLifetimeTo, otherEntry);
		};
		auto seqeunceHandler = [&](Node* node) {
			ds::amt::ImplicitSequence<TableEntry*> is = node->entries_;
			if (is.size() == 0) {
				return false;
			}
			//spustenie predikatu na IS
			Algorithm::matchAddressBy(is.begin(), is.end(), inserter, lifetimePredicate);
		};
		//spustenie sequenceHandler nad hierarchiou
		Algorithm::matchAddressBy(begin, end, [](Node* node) {}, seqeunceHandler);

		//Level 4
		sortSequence(matchedEntries);
		printEntries(matchedEntries);

		delete matchLifetimeFrom;
		delete matchLifetimeTo;
	}

	//Level 2
	void listOctets() {
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
			} catch (const std::logic_error&) {
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

	//Level 2
	void filterHierarchy(Hierarchy::PreOrderHierarchyIterator iterator,char choice) {
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

	//Level 2
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

	//Level 3
	void findByNexthop() {
		printf("Enter your next hop IP address: ");

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

		ds::amt::ImplicitSequence<TableEntry*>** sequence = nullptr;
		bool found = treap->treap_.tryFind(matchAddress.stringWithoutMask(), sequence);
		if (!found) {
			printf("No addresses found\n");
			return;
		}

		printf("Found addresses %d:\n",(*sequence)->size());
		for (auto entry : **sequence) {
			entry->print();
		}
	}

	void printEntries(ds::amt::ImplicitSequence<TableEntry*> matchedEntries) {
		matchedEntries.isEmpty() ? printf("No addresses found\n") : printf("Found addresses %d:\n", matchedEntries.size());
		int i = 1;
		for (auto entry : matchedEntries) {
			if (entry != nullptr) {
				printf("%d ", i);
				entry->print();
				i++;
			}
		}
		printf("\n");
	}

	//Level 1
	static bool matchWithAddress(IPAddress matchAddress, TableEntry* otherEntry) {
		if (otherEntry == nullptr) {
			return false;
		}
		int maskTmp = otherEntry->getAddress()->getMaskLength();
		for (size_t i = 0; i < 4; i++) {
			uint8_t otherOctet = otherEntry->getAddress()->getAddressOctet(i);
			uint8_t matchOctet = matchAddress.getAddressOctet(i);
			uint8_t mask = IPAddress::createMaskNumber(maskTmp > 8 ? 8 : maskTmp);
			if ((otherOctet & mask) != (matchOctet & mask)) {
				return false;
			}

			maskTmp -= 8;
			if (maskTmp <= 0) {
				break; 
			}
		}
		return true;
	}

	//Level 1
	static bool matchLifetime(Time* matchLifetimeFrom, Time* matchLifetimeTo, TableEntry* otherEntry) {
		if (otherEntry == nullptr) {
			return false;
		}
		if (*(otherEntry->getTime()) >= *matchLifetimeFrom && *(otherEntry->getTime()) <= *matchLifetimeTo) {
			return true;
		}
		return false;
	}

	//Level 4
	void sortSequence(ds::amt::ImplicitSequence<TableEntry*>& sequence) {
		Sorter<TableEntry*> sorter = Sorter<TableEntry*>();
		
		printf("How do you want to sort your results?\n");
		printf("Press enter to skip sorting\n");
		printf("By [t]ime/[o]ctets: ");
		std::string input;
		std::getline(std::cin, input);
		if (input.empty()) {
			printf("Skipping sorting\n");
			return;
		}

		if (input[0] == 't') {
			auto sortByTime = [&](TableEntry* first, TableEntry* second) { return Sorter<TableEntry*>::compareTime(first, second); };
			sorter.sortBy(sequence, sortByTime);
		} else if (input[0] == 'o') {
			auto sortByOctets = [&](TableEntry* first, TableEntry* second) { return Sorter<TableEntry*>::comparePrefix(first, second); };
			sorter.sortBy(sequence, sortByOctets);
		} else {
			printf("Skipping sorting\n");
		}
	}
};