#include "CardCollection.h"
#include <random>
#include <algorithm>
#include <vector>
using namespace std;
YGO::Card YGO::DefaultCardCollection::remove(int index)
{
	if (index >= cards.size() || index < 0) {
		panic("index out of range");
	}
	Card c = cards[index];
	cards.erase(cards.begin() + index);
	return c;
}
vector<YGO::Card> YGO::DefaultCardCollection::remove_all(vector<int> indices)
{
	std::sort(indices.begin(), indices.end());
	int k = 0;
	vector<Card> removed;
	deque<Card> new_cards;
	for (int i = 0; i < cards.size(); i++) {
		if (k < indices.size() && i == indices[k]) {
			removed.emplace_back(cards[i]);
			k++;
		}
		else {
			new_cards.emplace_back(cards[i]);
		}
	}
	cards = new_cards;
	return removed;
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
