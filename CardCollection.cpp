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
	int k = cards.size() / 2;
	auto mid = cards.begin();
	std::advance(mid, k);
	std::list<Card> tmp;
	tmp.splice(tmp.begin(), cards, cards.begin(), mid);
	cards.splice(cards.end(), tmp);
	//std::shuffle(cards.begin(), cards.end(), rd);
}
