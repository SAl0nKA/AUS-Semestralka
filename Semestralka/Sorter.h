#include <libds/adt/sorts.h>
#include <libds/amt/implicit_sequence.h>
#include "TableEntry.h"

#pragma once
template <class DataType>
class Sorter {
	ds::adt::QuickSort<DataType> sort;


public:
	Sorter() {

	}

	template <typename Comparator>
	void sortBy(ds::amt::ImplicitSequence<DataType>& sequence, Comparator comparator) {
		sort.sort(sequence,comparator);
	}

	static bool comparePrefix(TableEntry* first, TableEntry* second) {
		for (size_t i = 0; i < 4; i++) {
			if (first->getAddress()->getAddressOctet(i) < second->getAddress()->getAddressOctet(i)) {
				return true;
			}
			if (first->getAddress()->getAddressOctet(i) > second->getAddress()->getAddressOctet(i)) {
				return false;
			}
		}
		if (first->getAddress()->getMaskLength() < second->getAddress()->getMaskLength()) {
			return true;
		}
		return false;
	}

	static bool compareTime(TableEntry* first, TableEntry* second) {
		if (first->getTime()->getSeconds() < second->getTime()->getSeconds()) {
			return true;
		}
		return false;
	}
};

