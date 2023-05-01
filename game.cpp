#include "game.h"
#include "global.h"
using namespace std;
namespace YGO {
	YGO::Game::Game(Deck& deck_template, int start_hand_cards)
	{
		vector<Card> deck_cards = deck_template.generate();
		vector<Card> hand_cards(deck_cards.begin(), deck_cards.begin() + start_hand_cards);
		m_deck = new DefaultCardCollection(deck_cards.begin(), deck_cards.end());
		m_hand = new DefaultCardCollection(hand_cards.begin(), hand_cards.end());
		m_field = new DefaultCardCollection();
		m_bochi = new DefaultCardCollection();
		m_jyogai = new DefaultCardCollection();
	}

	bool YGO::Game::execute_hand_card(int index, int opt)
	{
		Executor e = Executor(this, this->m_hand, index, opt);
		return e.run();
	}

	YGO::Yisp::Object* YGO::Executor::execStatement(const t_string& s)
	{
		if (s == "@") {
			Card card = m_src->remove(m_card_index);
			m_game->m_bochi->push_front(card);
			return Yisp::Void::get();
		}

		return Yisp::Void::get();
	}

	YGO::Executor::Executor(Game* game, CardCollection* src, int card_index, int opt)
		:m_game(game), m_src(src), m_card_index(card_index), m_opt(opt)
	{

	}

	bool YGO::Executor::run()
	{
		Card card = m_src->get(m_card_index);
		vector<t_string> statements = split(card.program(), ";");
		for (auto statement : statements) {
			trim(statement);
			execStatement(statement);
		}
		return true;
	}
}
