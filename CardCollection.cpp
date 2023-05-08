#include "CardCollection.h"
#include <random>
#include <algorithm>
#include <vector>
using namespace std;
void YGO::DefaultCardCollection::move_to(YGO::CardNode& card_node, std::list<YGO::Card>& dst)
{
	dst.splice(dst.begin(), cards, card_node);
}
void YGO::DefaultCardCollection::move_to(YGO::CardNode& card_node, YGO::CardCollection& dst)
{
	std::list<Card> tmp;
	move_to(card_node, tmp);
	dst.push_back(tmp);
}

void YGO::DefaultCardCollection::shuffle()
{
	std::random_device rd;
	vector<list<Card>> segments;
	vector<int> segidxs;
	const int M = min(4, max<int>(1, cards.size() / 4));
	int i = 0;
	while (cards.size() > 0) {
		list<Card> segment;
		int m = min<int>(cards.size(), M);
		auto ed = cards.begin();
		advance(ed, m);
		segment.splice(segment.begin(), cards, cards.begin(), ed);
		segments.emplace_back(segment);
		segidxs.emplace_back(i++);
	}
	std::shuffle(segidxs.begin(), segidxs.end(), rd);
	for (auto i : segidxs) {
		cards.splice(cards.end(), segments[i]);
	}
}
