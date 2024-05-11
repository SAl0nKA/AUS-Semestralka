#pragma once
#include <libds/amt/explicit_hierarchy.h>
#include "Node.h"

class Hierarchy {
private:
	using HierarchyType = ds::amt::MultiWayExplicitHierarchy<Node>;
	using HierarchyBlockType = ds::amt::MultiWayExplicitHierarchyBlock<Node>;
	HierarchyType hierarchy_;

public:
	Hierarchy(std::vector<TableEntry*> rt_) {
		Node root = {0,nullptr};
		//root.octet_ = 0;
		hierarchy_.emplaceRoot().data_ = root;

		int curretOctetLevel = 0;
		HierarchyBlockType* lastNode = hierarchy_.accessRoot();
		int i = 1;
		for (TableEntry* entry : rt_) {
			//printf("%d ", i);
			IPAddress* tmp = entry->getAddress();
			
			for (size_t i = 0; i < 4; i++) {
				int tmpOctetValue = tmp->getAddressOctet(i);
				Node tmpNode;
				tmpNode.octet_ = tmpOctetValue;
				
				//std::cout << tmpNode.octet_ << ".";

				HierarchyBlockType* tmpParent = hierarchy_.accessLastSon(*lastNode);
				if (tmpParent == nullptr) {
					lastNode = &hierarchy_.emplaceLastSon(*lastNode);
					lastNode->data_.octet_ = tmpOctetValue;
					//printf("%hu.\n\t", tmpOctetValue);
					continue;
				}
				if (lastNode->data_.octet_ == tmpNode.octet_) {
					continue;
				}
				lastNode = &hierarchy_.emplaceLastSon(*tmpParent);
				lastNode->data_.octet_ = tmpOctetValue;
				//printf("%hu.", tmpOctetValue);
			}
			//std::cout << "\n";
			lastNode->data_.entry_ = entry;
			lastNode = hierarchy_.accessRoot();
			++i;
		}
		
	}
};

