#include "game.h"
#include "global.h"
#include "context.h"
#include <unordered_set>

using namespace std;
namespace YGO {
	Yisp::Void Yisp::Void::void_;
	Game::Game(Deck& deck_template, int start_hand_cards, const vector<Condition*> & wanted_conds)
		:m_wanted_conds(wanted_conds)
	{
		vector<Card> deck_cards = deck_template.generate();
		vector<Card> hand_cards(deck_cards.begin(), deck_cards.begin() + start_hand_cards);
		m_deck = make_shared<DefaultCardCollection>(deck_cards.begin(), deck_cards.end());
		m_hand = make_shared<DefaultCardCollection>(hand_cards.begin(), hand_cards.end());
		m_field = make_shared<DefaultCardCollection>();
		m_bochi = make_shared<DefaultCardCollection>();
		m_jyogai = make_shared<DefaultCardCollection>();
	}
	std::vector<int> Game::select_add_hand_card(const vector<Card>& cards, int k)
	{
		unordered_set<int> unselected_idx;
		for (int i = 0; i < cards.size(); i++) {
			unselected_idx.emplace(i);
		}

		vector<int> selected;
		for (int t = 0; t < k; t++) {
			for (auto it = m_wanted_conds.begin(); it != m_wanted_conds.end(); ++it) {
				for (auto i : unselected_idx) {
					if ((*it)->match(cards[i])) {
						selected.emplace_back(i);
						unselected_idx.erase(i);
					}
				}
			}
		}
		return selected;
	}

	bool YGO::Game::execute_hand_card(int index, int opt)
	{
		Executor e(this, this->m_hand, index, opt);
		return e.run();
	}

	void Yisp::CardSet::move_to_back(std::shared_ptr<CardCollection> dst) 
	{
		if (!valid) panic("use invalid carset");
		auto removed = collection->remove_all(cards_idx);
		dst->push_back(removed);
		valid = false;
	}

	void Yisp::CardSet::move_to_back(std::shared_ptr<CardCollection> dst, const std::vector<int>& indexs)
	{
		if (!valid) panic("use invalid carset");

		vector<int> collection_idx_to_remove;
		for (auto i : indexs) {
			collection_idx_to_remove.emplace_back(cards_idx[i]);
		}
		auto removed = collection->remove_all(collection_idx_to_remove);
		dst->push_back(removed);

		valid = false;
	}

	shared_ptr<Yisp::CardSet> Yisp::CardSet::subset(int n)
	{
		if (!valid) panic("use invalid carset");
		shared_ptr<Yisp::CardSet> sub = make_shared<Yisp::CardSet>();
		sub->collection = collection;
		for (int i = 0; i < n && i < cards_idx.size(); i++) {
			sub->cards_idx.push_back(cards_idx[i]);
		}
		valid = false;
		return sub;
	}
	shared_ptr<Yisp::CardSet> Yisp::CardSet::subset(t_string cond)
	{
		if (!valid) panic("use invalid carset");
		shared_ptr<Yisp::CardSet> sub = make_shared<Yisp::CardSet>();
		sub->collection = collection;
		Condition* cond_p = Utils::parseSingle(cond);
		for (int idx : cards_idx) {
			if (cond_p->match(collection->get(idx))) {
				sub->cards_idx.push_back(idx);
			}
		}
		delete cond_p;
		valid = false;
		return sub;
	}

	vector<Card> Yisp::CardSet::get_cards()
	{
		if (!valid) panic("use invalid carset");
		vector<Card> cards;
		for (auto i : cards_idx) {
			cards.emplace_back(collection->get(i));
		}
		return cards;
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
			if (c2 == '>' || c2 == '+' || c2 == '-' || c2 == 'r') {
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

		//check if the function is forbidden
		if (m_forbidden_funcs[c]) {
			m_cond_break = true;
			return Yisp::Void::get();
		}

		switch (c)
		{
		//draw
		case '%':
		{
			if (params.size() != 1) {
				panic("wrong number of parameters for %");
			}
			int draw_count = std::dynamic_pointer_cast<Yisp::Number>(params[0])->num;
			auto drawed_cards = m_game->m_deck->front(draw_count);
			m_game->m_deck->pop_front(draw_count);
			m_game->m_hand->push_back(drawed_cards);
		}
		break;
		//move
		case '#':
		{
			if (params.size() != 2) {
				panic("wrong number of parameters for %");
			}
			auto src = std::dynamic_pointer_cast<Yisp::CardSet>(params[0]);
			auto dst = std::dynamic_pointer_cast<Yisp::CardSet>(params[1]);
			src->move_to_back(dst->collection);
		}
		break;
		//select
		case '$':
		{
			if (params.size() != 3) {
				panic("wrong number of parameters for %");
			}
			auto src = std::dynamic_pointer_cast<Yisp::CardSet>(params[0]);
			int cnt = std::dynamic_pointer_cast<Yisp::Number>(params[1])->num;
			auto dst = std::dynamic_pointer_cast<Yisp::CardSet>(params[2]);
			auto to_select = src->get_cards();
			auto selected_idxs = m_game->select_add_hand_card(to_select, cnt);
			src->move_to_back(dst->collection, selected_idxs);
		}
		break;
		//forbid
		case '!':
		{

		}
		break;
		//optional
		case '?':
		{

		}
		break;
		//assign
		case '=':
		{

		}
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
			shared_ptr<Yisp::CardSet> card_set(execSet(s));
			if (s.get() != '|') {
				panic("| not match in execNumber");
			}
			return make_shared<Yisp::Number>(card_set->collection->size());
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
			case 'r':
				res = random_int(num1, num2);
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
		t_string w = read_while(s, [this](char c) {
			return m_set_allowed_chars[c];
			});
		vector<t_string> ws = split(w, ".");
		shared_ptr<CardCollection> collection = nullptr;

		if (ws[0] == "H") {
			collection = m_game->m_hand;
		}
		else if (ws[0] == "D") {
			collection = m_game->m_deck;
		}
		else if (ws[0] == "B") {
			collection = m_game->m_bochi;
		}
		else if (ws[0] == "F") {
			collection = m_game->m_field;
		}
		else if (ws[0] == "J") {
			collection = m_game->m_jyogai;
		}
		else {
			panic("unknown collection: " + ws[0]);
		}
		shared_ptr<Yisp::CardSet> card_set(make_shared<Yisp::CardSet>(collection));
		//TODO
		for (int i = 1; i < ws.size(); i++) {
			auto w = ws[i];
			if (w[0] >= '0' && w[0] <= '9') {
				stringstream temp_ss;
				temp_ss << w;
				int u;
				temp_ss >> u;
				card_set = card_set->subset(u);
			}
			else {
				card_set = card_set->subset(w);
			}
		}
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
		m_set_allowed_chars['I'] = true;
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
