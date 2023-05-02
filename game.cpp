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
			s.get(c);
			return execCondition(s);
		}

		return execExpr(s);
	}

	shared_ptr<Yisp::Object> Executor::execExpr(std::stringstream& s)
	{
		char c = s.peek();
		//set
		if (c >= 'A' && c <= 'Z') {
			return execSet(s);
		}
		// number case 1, 3, 4
		if (c >= '0' && c <= '9' || c >= 'a' && c <= 'z' || c == '|') {
			return execNumber(s);
		}
		
		if (c == '(') {
			c = s.get();
			remove_space(s);
			char c2 = s.peek();
			// number case 2
			if (c2 == '>' || c2 == '+' || c2 == '-') {
				s.putback(c);
				return execNumber(s);
			}
			// function
			return execFunc(s);
		}
		else { // String
			string str = read_while(s, [this](char c) {
				return !std::isspace(c) && c != '(' && c != ')';
			});
			return make_shared<Yisp::String>(str);
		}
		return Yisp::Void::get();
	}

	shared_ptr<Yisp::Object> Executor::execFunc(std::stringstream& s)
	{
		char c = s.get();
		remove_space(s);
		vector< shared_ptr<Yisp::Object> > params;
		while (s.peek() != ')') {
			params.push_back(execExpr(s));
			remove_space(s);
			if (s.peek() == std::char_traits<char>::eof()) {
				panic("Unexpected eof in execFunc");
			}
		}
		s.get(); //read ')'

		switch (c) {
		case '%':
			break;
		case '#':
			break;
		case '$':
			break;
		case '!':
			break;
		case '?':
			break;
		case '=':
			break;
		default:
			panic("unknown function : " + c);
		}
		return Yisp::Void::get();
	}

	shared_ptr<Yisp::Number> Executor::execNumber(std::stringstream& s)
	{
		remove_space(s);
		char c;
		s.get(c);
		if (c >= '0' && c <= '9') {
			s.unget();
			int num;
			s >> num;
			return make_shared<Yisp::Number>(num);
		}
		if (c >= 'a' && c <= 'z') {
			return make_shared<Yisp::Number>(m_vars[c]);
		}
		if (c == '|') {
			shared_ptr<Yisp::CardSet> card_set = execSet(s);
			if (s.get() != '|') {
				panic("| not match in execNumber");
			}
			return make_shared<Yisp::Number>(card_set->c->size());
		}
		if (c == '(') {
			remove_space(s);
			s.get(c);
			remove_space(s);
			int num1 = execNumber(s)->num;
			remove_space(s);
			int num2 = execNumber(s)->num;
			int res;
			switch (c) {
			case '+':
				res = num1 + num2;
				break;
			case '-':
				res = num1 - num2;
				break;
			case '>':
				res = num1 > num2;
				break;
			}
			remove_space(s);
			s.get(c);
			if (c != ')') {
				panic(") not match in execNumber");
			}
			return make_shared<Yisp::Number>(res);
		}
		return make_shared<Yisp::Number>(0);
	}

	std::shared_ptr<Yisp::CardSet> Executor::execSet(std::stringstream& s)
	{
		remove_space(s);
		shared_ptr<Yisp::CardSet> card_set = make_shared<Yisp::CardSet>();
		t_string w = read_while(s, [this](char c) {
			return m_set_allowed_chars[c];
		});
		shared_ptr<CardCollection> collection = nullptr;
		if (w[0] == 'H') {
			collection = m_game->m_hand;
		}
		else if (w[0] == 'D') {
			collection = m_game->m_deck;
		}
		else if (w[0] == 'B') {
			collection = m_game->m_bochi;
		}
		else if (w[0] == 'F') {
			collection = m_game->m_field;
		}
		else if (w[0] == 'J') {
			collection = m_game->m_jyogai;
		}
		//TODO
		card_set->c = collection;
		return card_set;
	}

	shared_ptr<Yisp::Object> Executor::execCondition(std::stringstream& s)
	{
		//TODO
		return Yisp::Void::get();
	}

	YGO::Executor::Executor(Game* game, shared_ptr<CardCollection> src, int card_index, int opt)
		:m_game(game), m_src(src), m_card_index(card_index), m_opt(opt), m_cond_break(false), m_vars(128),
		m_set_allowed_chars(256)
	{
		for (char c = 'A'; c <= 'Z'; c++) {
			m_set_allowed_chars[c] = true;
		}
		for (char c = 'a'; c <= 'z'; c++) {
			m_set_allowed_chars[c] = true;
		}
		for (char c = '0'; c <= '9'; c++) {
			m_set_allowed_chars[c] = true;
		}
		m_set_allowed_chars['-'] = true;
		m_set_allowed_chars[':'] = true;
		m_set_allowed_chars['.'] = true;
	}

	bool YGO::Executor::run()
	{
		Card card = m_src->get(m_card_index);
		vector<t_string> statements = split(card.program(), ";");
		for (auto statement : statements) {
			trim(statement);
			stringstream ss(statement);
			execStatement(ss);
			if (m_cond_break) {
				return false;
			}
		}
		return true;
	}
}
