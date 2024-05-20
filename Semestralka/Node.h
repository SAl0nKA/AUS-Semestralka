#pragma once
#include <iostream>
#include "TableEntry.h"
#include <libds/heap_monitor.h>

struct Node {
	std::uint8_t octet_;
	//TableEntry* entry_;
	ds::amt::ImplicitSequence<TableEntry*> entries_;

	bool operator==(const Node& otherNode) {
		return this->octet_ == otherNode.octet_ && this->entries_.equals(otherNode.entries_);
	}
};