#include "CardCollection.h"
#include <random>
#include <algorithm>
YGO::Card YGO::DefaultCardCollection::remove(int index)
{
	if (index >= 0) {
		auto it = cards.erase(cards.begin() + index);
		return *it;
	}
	auto it = cards.erase(cards.end() + index);
	return *it;
}
const YGO::Card& YGO::DefaultCardCollection::get(int index)
{
	if (index >= 0) {
		return cards[index];
	}
	auto it = cards.end() + index;
	return *it;
}
void YGO::DefaultCardCollection::shuffle()
{
	std::random_device rd;
	std::shuffle(cards.begin(), cards.end(), rd);
}
