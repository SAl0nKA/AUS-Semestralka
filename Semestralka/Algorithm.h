#pragma once
class Algorithm {
public:
	template <class Iterator, typename Predicate, typename InsertMatch>
	static void matchAddressBy(Iterator begin, Iterator end, InsertMatch inserter, Predicate predicate) {
		while (begin != end) {
			auto item = begin;
			if (predicate(*item)) {
				inserter(*item);
			}
			++begin;
		}
	}
};

