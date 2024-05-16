#pragma once
class Algorithm {
public:
	template <class Iterator, typename Predicate, typename InsertMatch, typename MatchStructure>
	static void matchAddressBy(Iterator begin, Iterator end, InsertMatch inserter, MatchStructure structure, Predicate predicate) {
		while (begin != end) {
			auto item = begin;
			if (predicate(*item)) {
				inserter(*item,structure);
			}
			++begin;
		}
	}
};

