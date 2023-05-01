#include "game.h"
#include "global.h"

using namespace std;
namespace YGO {
	Yisp::Void Yisp::Void::void_;
	Game::Game(Deck& deck_template, int start_hand_cards)
	{
		vector<Card> deck_cards = deck_template.generate();
		vector<Card> hand_cards(deck_cards.begin(), deck_cards.begin() + start_hand_cards);
		m_deck = make_shared<DefaultCardCollection>(deck_cards.begin(), deck_cards.end());
		m_hand = make_shared<DefaultCardCollection>(hand_cards.begin(), hand_cards.end());
		m_field = make_shared<DefaultCardCollection>();
		m_bochi = make_shared<DefaultCardCollection>();
		m_jyogai = make_shared<DefaultCardCollection>();
	}

	bool YGO::Game::execute_hand_card(int index, int opt)
	{
		Executor e(this, this->m_hand, index, opt);
		return e.run();
	}

	

	shared_ptr<Yisp::Object> Executor::execStatement(stringstream& s)
	{
		char c = s.peek();
		if (c == '@') {
			Card card = m_src->remove(m_card_index);
			m_game->m_bochi->push_front(card);
			return Yisp::Void::get();
		}
		if (c == '/') {
			//condition
			return Yisp::Void::get();
		}

		return execExpr(s);
	}

	shared_ptr<Yisp::Object> Executor::execExpr(std::stringstream& s)
	{
		return Yisp::Void::get();
	}

	shared_ptr<Yisp::Number> Executor::execNumber(std::stringstream& s)
	{
		return make_shared<Yisp::Number>(0);
	}

	YGO::Executor::Executor(Game* game, shared_ptr<CardCollection> src, int card_index, int opt)
		:m_game(game), m_src(src), m_card_index(card_index), m_opt(opt)
	{

	}

	bool YGO::Executor::run()
	{
		Card card = m_src->get(m_card_index);
		vector<t_string> statements = split(card.program(), ";");
		for (auto statement : statements) {
			trim(statement);
		}
		return true;
	}
}
