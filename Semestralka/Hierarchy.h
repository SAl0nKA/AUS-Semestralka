#pragma once
#include <libds/amt/explicit_hierarchy.h>
#include "Node.h"
#include <libds/heap_monitor.h>

class Hierarchy {
public:
	using HierarchyType = ds::amt::MultiWayExplicitHierarchy<Node*>;
	using HierarchyBlockType = ds::amt::MultiWayExplicitHierarchyBlock<Node*>;
	using PreOrderHierarchyIterator = ds::amt::Hierarchy<ds::amt::MWEHBlock<Node*>>::PreOrderHierarchyIterator;

	HierarchyType hierarchy_;

public:
	Hierarchy() {
		Node* root = new Node{NULL,ds::amt::ImplicitSequence<TableEntry*>()};
		hierarchy_.emplaceRoot().data_ = root;
	}

	~Hierarchy() {
		hierarchy_.processPostOrder(hierarchy_.accessRoot(), [](HierarchyBlockType* block) { delete block->data_; });
		hierarchy_.clear();
	}

	void addToHierarchy(TableEntry* entry) {
		HierarchyBlockType* lastNode = hierarchy_.accessRoot();

		IPAddress* tmp = entry->getAddress();

		for (size_t i = 0; i < 4; i++) {
			//nacitanie hodnoty octetu a vytvorenie nodu s danou hodnotou
			std::uint8_t tmpOctetValue = tmp->getAddressOctet(i);
			Node* tmpNode = new Node{ tmpOctetValue ,ds::amt::ImplicitSequence<TableEntry*>() };

			//ak nema node syna vytvori ho
			if (hierarchy_.accessLastSon(*lastNode) == nullptr) {
				lastNode = &hierarchy_.emplaceLastSon(*lastNode);
				lastNode->data_ = tmpNode;
				continue;
			}
			//ak ma syna porovna ho s pridavanou hodnotou, ak sa rovna tak nevytvara noveho a pouzije existujuceho
			if (hierarchy_.accessLastSon(*lastNode)->data_->octet_ == tmpNode->octet_) {
				lastNode = hierarchy_.accessLastSon(*lastNode);
				delete tmpNode;
				continue;
			}
			//ak sa nerovna tak vytvori noveho syna
			lastNode = &hierarchy_.emplaceLastSon(*lastNode);
			lastNode->data_ = tmpNode;
		}

		lastNode->data_->entries_.insertLast().data_ = entry;
		lastNode = hierarchy_.accessRoot();
	}
};

