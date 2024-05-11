#pragma once
#include <iostream>
#include "TableEntry.h"
struct Node {
	std::uint8_t octet_;
	TableEntry* entry_;
};

bool operator==(const Node& node, const Node& otherNode) {
	return node.octet_ == otherNode.octet_ && node.entry_== otherNode.entry_;
}

