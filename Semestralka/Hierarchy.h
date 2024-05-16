#pragma once
#include <libds/amt/explicit_hierarchy.h>
#include "Node.h"

class Hierarchy {
public:
	using HierarchyType = ds::amt::MultiWayExplicitHierarchy<Node*>;
	using HierarchyBlockType = ds::amt::MultiWayExplicitHierarchyBlock<Node*>;
	//using PreOrderHierarchyIterator = ds::amt::Hierarchy<Node*>::PreOrderHierarchyIterator;
	//using PreOrderHierarchyIterator = ds::amt::Hierarchy<ds::amt::HierarchyBlock>::PreOrderHierarchyIterator;
	using PreOrderHierarchyIterator = ds::amt::Hierarchy<ds::amt::MWEHBlock<Node*>>::PreOrderHierarchyIterator;

	HierarchyType hierarchy_;

public:
	Hierarchy(std::vector<TableEntry*> rt_) {
		Node* root = new Node{NULL,nullptr};
		hierarchy_.emplaceRoot().data_ = root;
		int i = 0;
		HierarchyBlockType* lastNode = hierarchy_.accessRoot();
		for (TableEntry* entry : rt_) {
			IPAddress* tmp = entry->getAddress();
			
			for (size_t i = 0; i < 4; i++) {
				//nacitanie hodnoty octetu a vytvorenie nodu s danou hodnotou
				std::uint8_t tmpOctetValue = tmp->getAddressOctet(i);
				Node* tmpNode = new Node{tmpOctetValue ,nullptr};
				
				//ak nema node syna vytvori ho
				if (hierarchy_.accessLastSon(*lastNode) == nullptr) {
					lastNode = &hierarchy_.emplaceLastSon(*lastNode);
					lastNode->data_ = tmpNode;
					continue;
				}
				//ak ma syna porovna ho s pridavanou hodnotou, ak sa rovna tak nevytvara noveho a pouzije existujuceho
				if (hierarchy_.accessLastSon(*lastNode)->data_->octet_ == tmpNode->octet_) {
					lastNode = hierarchy_.accessLastSon(*lastNode);
					continue;
				}
				//ak sa nerovna tak vytvori noveho syna
				lastNode = &hierarchy_.emplaceLastSon(*lastNode);
				lastNode->data_ = tmpNode;
			}
			//std::cout << i << " ";
			lastNode->data_->entry_ = entry;
			//lastNode->data_->entry_->print();
			lastNode = hierarchy_.accessRoot();
			i++;
		}
		
	}

	void visualize(HierarchyBlockType* node, std::string indent = "") {
		if (!node) return;

		TableEntry* tmp = node->data_->entry_;
		std::cout << indent;
		printf("Octet: %hu\n", node->data_->octet_);
		if (tmp != nullptr) {
			std::cout << "Entry: ";
			tmp->print();
			std::cout << std::endl;
		} else {
			
			//std::cout << "Octet: " << node->data_.octet_ << std::endl;
		}

		auto sons = hierarchy_.getSons(*node);
		if (sons) {
			for (auto son : *sons) {
				visualize(son, indent + "  ");
			}
		}
	}
};

