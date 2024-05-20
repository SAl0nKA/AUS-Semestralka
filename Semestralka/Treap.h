#pragma once
#include <libds/adt/table.h>

class Treap {
public:
	using NextHopTreap = ds::adt::Treap<std::string, ds::amt::ImplicitSequence<TableEntry*>*>;
	NextHopTreap treap_;

	~Treap() {
		for (auto node : treap_) {
			delete node.data_;
		}
	}

	void addToTreap(TableEntry* entry) {
		std::string nextHopStr = entry->getNextHop()->stringWithoutMask();
		ds::amt::ImplicitSequence<TableEntry*>** sequence = nullptr;
		bool found = treap_.tryFind(nextHopStr, sequence);
		if (found) {
			(*sequence)->insertLast().data_ = entry;
		} else {
			ds::amt::ImplicitSequence<TableEntry*>* tmp = new ds::amt::ImplicitSequence<TableEntry*>();
			tmp->insertLast().data_ = entry;
			treap_.insert(nextHopStr, tmp);
		}
	}
};

