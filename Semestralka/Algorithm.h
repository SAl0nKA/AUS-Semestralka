#pragma once
class Algorithm {
public:
	template <class Iterator, typename Predicate, class MatchesStructure, typename InsertMatch>
	static void matchAddressBy(Iterator begin, Iterator end, MatchesStructure& matches, InsertMatch inserter, Predicate predicate) {
		while (begin != end) {
			auto item = begin;
			if (predicate(**item)) {
				inserter(matches, **item);
			}
			++begin;
		}
	}
};

