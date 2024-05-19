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
};

