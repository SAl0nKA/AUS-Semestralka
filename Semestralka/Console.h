#pragma once
#include <vector>
#include "TableEntry.h"
#include "IPAddress.h"
#include <iostream>
#include <limits>
#include "Hierarchy.h"
#include "RTParser.h"

class Console {
private:
	std::vector<TableEntry*> rt_;
	Hierarchy* hierarchy;

public:
	Console(std::string path) {
		printf("Welcome to the routing table parser\n");
		printf("Input the path for the CSV file or press enter for default path [./RT.csv]: ");
		std::string input = "";
		//std::cin >> input;
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
			/*std::cout << i << " ";
			entry->print();*/
			rt_.push_back(entry);
			++i;
		}
		hierarchy = new Hierarchy(rt_);

	}
public:
	bool mainMenu() {
		printf("[1] List all available entries\n");
		printf("[2] Find entry by IP\n");
		printf("[3] Find entry by lifetime\n");
		printf("[4] List available first octets\n");
		printf("[5] \n");
		printf("[6] Exit\n");
		printf("Input: ");

		std::string input;
		std::getline(std::cin, input);

		switch (input[0]) {
		case '1':
			listAll();
			break;
		case '2':
			findByIP();
			break;
		case '3':
			findByLifetime();
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

	void findByIP() {
		printf("Enter your IP address: ");

		std::string inputIP;
		std::getline(std::cin, inputIP);

		if (inputIP.empty()) {
			printf("Invalid choice\n");
			return;
		}

		std::vector<TableEntry*> matchedAddresses;
		IPAddress matchAddress(inputIP);

		auto matchWithAddressPredicate = [&matchAddress](TableEntry* otherEntry) {
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
			};

		Algorithm::matchAddressBy(rt_.begin(), rt_.end(),
			[&](TableEntry* entry) {
				matchedAddresses.push_back(entry);
			}, matchWithAddressPredicate);

		matchedAddresses.empty() ? std::cout << "No addresses found\n" : std::cout << "Found address matches\n";
		for (auto& entry : matchedAddresses) {
			entry->print();
		}
	}

	void findByLifetime() {
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

		std::vector<TableEntry*> matchesLifetime;

		Time* matchLifetimeFrom = inputFrom.empty() ? new Time(0) : new Time(inputFrom);
		Time* matchLifetimeTo = inputTo.empty() ? new Time(std::numeric_limits<int>::max()) : new Time(inputTo);


		auto matchLifetimePredicate = [&matchLifetimeFrom, &matchLifetimeTo](TableEntry* otherEntry) {
			if (*(otherEntry->getTime()) >= *matchLifetimeFrom && *(otherEntry->getTime()) <= *matchLifetimeTo) {
				return true;
			}
			return false;
			};



		Algorithm::matchAddressBy(rt_.begin(), rt_.end(),
			[&](TableEntry* entry) {
				matchesLifetime.push_back(entry);
			}, matchLifetimePredicate);


		std::cout << "Found lifetime matches\n";
		for (auto& entry : matchesLifetime) {
			entry->print();
		}
	}

	void listOctets() {
		/*hierarchy->visualize(hierarchy->hierarchy_.accessRoot());
		return;*/
		auto iterator = hierarchy->hierarchy_.beginPre();

		int sonsCount = listAvailableOctets(iterator);
		
		std::string input;
		int index;
		int currentLevel = 0;
		
		while (currentLevel != -1) {
			if (currentLevel != 0) {
				printf("Current octet: %d\n", currentLevel);
			}
			printf("Select octet index or use -1 to move up a level: ");
			std::getline(std::cin, input);

			index = std::stoi(input);
			if (index == -1) {
				currentLevel--;
				if (currentLevel == -1) {
					printf("Exiting\n");
					break;
				}
				iterator.goToParent();
				sonsCount = listAvailableOctets(iterator);
				continue;
			}
			if (index >= sonsCount) {
				printf("Invalid index\n");
				continue;
			}
			currentLevel++;
			iterator.goToNthSon(index);
			if (currentLevel == 4) {
				auto block = iterator.getBlockType().data_;
				//printf("%hu\n", block->octet_);
				//TableEntry* entry = iterator.getBlockType().data_->entry_;
				if (block->entry_ == nullptr) {
					printf("There's an empty entry\n");
				} else {
					block->entry_->print();
				}
				currentLevel--;
				iterator.goToParent();
				sonsCount = listAvailableOctets(iterator);
				continue;
			} 
			sonsCount = listAvailableOctets(iterator);
		}
	}

	//void filterHierarchy(ds::amt::MultiWayExplicitHierarchy<Hierarchy::HierarchyBlockType>::PreOrderHierarchyIterator iterator) {
	//	auto iterBegin = ds::amt::Hierarchy<Node*>::PreOrderHierarchyIterator(&hierarchy->hierarchy_, iterator.getBlockType());
	//	//auto itBegin = ds::amt::MultiWayExplicitHierarchy<Node*>::PreOrderHierarchyIterator(&hierarchy, &iterator.getBlockType());
	//	auto iterEnd = ds::amt::Hierarchy<Node*>::PreOrderHierarchyIterator(&hierarchy->hierarchy_,nullptr);
	//}

	int listAvailableOctets(ds::amt::Hierarchy<Hierarchy::HierarchyBlockType>::PreOrderHierarchyIterator& iterator) {
		//auto iterator = hierarchy->hierarchy_.beginPre();
		auto sons = iterator.getBlockType().sons_;
		printf("Available octets: \n");
		size_t i = 0;
		for (auto son : *sons) {
			printf("[%d]%hu ", i, son->data_->octet_);
			++i;
		}
		printf("\n");
		return sons->size();
	}
};

