#include <random>
#include <unordered_set>
#include "game.h"
#include "global.h"
#include "context.h"


using namespace std;
namespace YGO {
	Yisp::Void Yisp::Void::void_;
	Game::Game(const Deck& deck_template, const Simulator::Topic &topic)
		:m_forbidden_funcs(256), m_used_funcs(256), m_vars(256), 
		m_wanted_hand_conds(topic.get_wanted_hand_conds()),
		m_wanted_grave_conds(topic.get_wanted_grave_conds()),
		m_header(topic.m_header)
	{
		int start_hand_cards = topic.m_start_card;
		
		vector<Card> deck_cards = deck_template.generate();

		std::random_device rd;
		std::shuffle(deck_cards.begin(), deck_cards.end(), rd);

		vector<Card> hand_cards(deck_cards.begin(), deck_cards.begin() + start_hand_cards);
		for (auto& c : hand_cards) {
			for (auto cond : m_wanted_hand_conds) {
				if (cond->match(c)) {
					m_wanted_hand_conds.erase(cond);
					break;
				}
			}
		}
		m_deck = make_shared<DefaultCardCollection>("D", deck_cards.begin() + start_hand_cards, deck_cards.end());
		m_name_to_collection["D"] = m_deck;
		m_hand = make_shared<DefaultCardCollection>("H", hand_cards.begin(), hand_cards.end());
		m_name_to_collection["H"] = m_hand;
		m_field = make_shared<DefaultCardCollection>("F");
		m_name_to_collection["F"] = m_field;
		m_bochi = make_shared<DefaultCardCollection>("B");
		m_name_to_collection["B"] = m_bochi;
		m_jyogai = make_shared<DefaultCardCollection>("J");
		m_name_to_collection["J"] = m_jyogai;

	}


	void Game::run() {
		if (m_header.size()) {
			Executor e(this, this->m_hand, this->m_hand->end(), 0);
			e.run_header(m_header);
		}
		cout << "-----------run-------------" << endl;
		int executed_count = 0;
		bool cont = true;
		while(cont)
		{
			loop_begin:
			cont = false;
			for (auto col : { m_hand, m_bochi }) {
				for (auto it = col->begin(); it != col->end(); ++it) {
					Card c = *it;
					for (int i = 0; i < c.m_effects.size(); i++) {
						if (!c.m_effects[i].is_executable_at(col->name())) {
							//printf("skip\n");
							continue;
						}
						//printf("%s %c, %c\n",c.name().c_str(), c.m_effects[i].m_valid_position[0], col->name()[0]);
						if (c.m_effects[i].exec_once_each_turn()) {
							if (m_already_executed.count(c.effect_name(i))) {
								//cout << "cannot exec because of [1]" << endl;
								continue;
							}
						}
						if (c.m_effects[i].exec_at_beginning() && executed_count != 0) {
							//cout << "cannot exec because of [^]" << endl;
							continue;
						}
						cont = execute_card(col, it, i);
						if (cont) {
							//success to execute
							cout << "executed card " << c.effect_name(i) << " [" << c.description() << "]" << endl;
							m_already_executed.insert(c.effect_name(i));
							executed_count++;
							goto loop_begin;
						}
						//failed to execute, continue
					}
				}
			}
		}
	}
	void Game::select_add_hand_card(Yisp::CardSet& to_select, int k, std::shared_ptr<CardCollection>& dst)
	{
		list<Card> selected;
		for (int t = 0; t < k; t++) {
			for (auto cond_it = m_wanted_hand_conds.begin(); cond_it != m_wanted_hand_conds.end(); ++cond_it) {
				for (auto card_node_it = to_select.cards_its.begin(); card_node_it != to_select.cards_its.end(); card_node_it++ ) {
					if ((*cond_it)->match(**card_node_it)) {
						to_select.collection->move_to(*card_node_it, selected);
						to_select.cards_its.erase(card_node_it);
						goto next_card;
					}
				}
			}
			//fallback
			to_select.collection->move_to(*to_select.cards_its.begin(), selected);
			to_select.cards_its.erase(to_select.cards_its.begin());
			
		next_card:;
		}
		if (selected.size() != k) {
			panic("bug: selected size unmatched");
		}
		dst->push_back(selected);
	}

	bool YGO::Game::execute_card(std::shared_ptr<CardCollection> col, CardNode it, int opt)
	{
		Executor e(this, col, it, opt);
		return e.run();
	}

	std::shared_ptr<CardCollection> YGO::Game::find_card_position(CardNode card_it) {
		for (auto c : { m_hand, m_field, m_bochi, m_jyogai, m_deck }) {
			for (auto it = c->begin(); it != c->end(); ++it) {
				if (card_it == it) {
					return c;
				}
			}
		}
		return shared_ptr<CardCollection>();
	}

	//------------------------CardSet-----------------------------//

	std::ostream& operator<<(std::ostream& os, const Yisp::CardSet& c) {
		os << "{";
		for (auto it : c.cards_its) {
			os << it->print_name() << ", ";
		}
		os << "}";
		return os;
	}

	void Yisp::CardSet::move_to_back(std::shared_ptr<CardCollection> dst) 
	{
		list<Card> tmp;
		for (auto it : cards_its) {
			collection->move_to(it, tmp);
		}
		dst->push_back(tmp);
	}

	shared_ptr<Yisp::CardSet> Yisp::CardSet::subset(int n)
	{
		shared_ptr<Yisp::CardSet> sub = make_shared<Yisp::CardSet>();
		sub->collection = collection;
		auto card_node_it = cards_its.begin();
		for (int i = 0; i < n && i < cards_its.size(); i++) {
			sub->cards_its.push_back(*card_node_it);
			card_node_it++;
		}
		return sub;
	}
	shared_ptr<Yisp::CardSet> Yisp::CardSet::subset(t_string cond)
	{
		shared_ptr<Yisp::CardSet> sub = make_shared<Yisp::CardSet>();
		sub->collection = collection;
		Condition* cond_p = Utils::parseSingle(cond);
		for (auto it : cards_its) {
			if (cond_p->match(*it)) {
				sub->cards_its.push_back(it);
			}
		}
		delete cond_p;
		return sub;
	}


	//------------------------Executor-----------------------------//

	shared_ptr<Yisp::Object> Executor::execStatement(stringstream& s)
	{
		//cout << "execStatement(" + s.str() + ")" << endl;
		char c = s.peek();
		if (c == '@') {
			m_activated = true;
			return Yisp::Void::get();
		}
		if (c == '/') {
			//condition
			s.get(c);
			shared_ptr<Yisp::Number> res = execCondition(s);
			if (res->num == 0) {
				m_cond_break = true;
			}
			return Yisp::Void::get();
		}
		if (c == '\\') {
			panic("should not use \\, check: " + s.str());
		}

		return execExpr(s);
	}

	std::shared_ptr<Yisp::String> Executor::execString(std::stringstream& s) {
		if (s.peek() == '\"') {
			s.get();
		}
		string str = read_while(s, [this](char c) {
			return !std::isspace(c) && c != '(' && c != ')';
			});
		return make_shared<Yisp::String>(str);
	}

	vector< shared_ptr<Yisp::Object> > Executor::parseParams(stringstream& s, vector<parser_t> parserFuncs)
	{
		vector< shared_ptr<Yisp::Object> > params;
		int i = 0;
		while (s.peek() != ')' && i < parserFuncs.size()) {
			params.push_back(parserFuncs[i++](this, s));
			remove_space(s);
			if (s.peek() == std::char_traits<char>::eof()) {
				panic("Unexpected eof in execFunc");
			}
		}
		remove_space(s);
		if (i != parserFuncs.size() || s.get() != ')') {
			panic("Unmatched ) or wrong number of arguments: " + s.str());
		}
		return params;
	}

	shared_ptr<Yisp::Object> Executor::execExpr(std::stringstream& s)
	{
		char c = s.get();
		if (c != '(') {
			panic("wrong expression: " + s.str());
		}
		if (s.peek() == ')') {
			//empty expression, do nothing
			return Yisp::Void::get();
		}
		t_string f;
		s >> f;
		remove_space(s);
		vector< shared_ptr<Yisp::Object> > params;

		//check if the function is forbidden
		if (m_game->m_forbidden_funcs.count(f)) {
			m_cond_break = true;
			return Yisp::Void::get();
		}
		
		
		if(f == "%") //draw
		{
			params = parseParams(s, { &Executor::execNumber });
			int draw_count = std::dynamic_pointer_cast<Yisp::Number>(params[0])->num;
			list<Card> drawed_cards;
			m_game->m_deck->pop_front(draw_count, drawed_cards);
			m_game->m_hand->push_back(drawed_cards);
			//printf("draw %d\n", draw_count);
		}
		else if(f == "#") //move
		{
			params = parseParams(s, { &Executor::execSet, &Executor::execSet });
			auto src = std::dynamic_pointer_cast<Yisp::CardSet>(params[0]);
			auto dst = std::dynamic_pointer_cast<Yisp::CardSet>(params[1]);

			if (src->size() == 0) {
				panic("# source should not be empty, check program: " + s.str());
			}

			//printf("move %d\n", (int)src->size());
			cout << "move " << *src << "from " << src->collection->name() << " to " << dst->collection->name() << endl;
			src->move_to_back(dst->collection);
		}
		else if (f == "$") //select
		{
			params = parseParams(s, { &Executor::execSet, &Executor::execNumber, &Executor::execSet });
			auto src = std::dynamic_pointer_cast<Yisp::CardSet>(params[0]);
			int cnt = std::dynamic_pointer_cast<Yisp::Number>(params[1])->num;
			auto dst = std::dynamic_pointer_cast<Yisp::CardSet>(params[2]);
			m_game->select_add_hand_card(*src, cnt, dst->collection);
			src->move_to_back(src->collection);
		}
		else if (f == "!")//forbid
		{
			params = parseParams(s, { &Executor::execString });
			auto op = std::dynamic_pointer_cast<Yisp::String>(params[0])->s;
			m_game->m_forbidden_funcs.insert(op);
		}
		else if (f == "=") //assign
		{
			params = parseParams(s, { &Executor::execString, &Executor::execNumber });
			string x = std::dynamic_pointer_cast<Yisp::String>(params[0])->s;
			int val = std::dynamic_pointer_cast<Yisp::Number>(params[1])->num;
			if (x[0] >= 'a' && x[0] <= 'z' || x[0] >= 'A' && x[0] <= 'Z') {
				if (x.size() == 1) {
					m_vars[x[0]] = val;
				}
				else {
					m_game->m_vars[x] = val;
				}
			}
			else {
				panic("error var name: " + x);
			}
		}
		else if (f == "if") {
			auto cond = execNumber(s);
			remove_space(s);
			if (cond->num) {
				auto res = execExpr(s);
				remove_space(s);
				execNothing(s);
				remove_space(s);
				if (s.get() != ')') {
					panic("Unmatched ) or wrong number of arguments: " + s.str());
				}
				return res;
			}
			else {
				execNothing(s);
				remove_space(s);
				auto res = execExpr(s);
				remove_space(s);
				if (s.get() != ')') {
					panic("Unmatched ) or wrong number of arguments: " + s.str());
				}
				return res;
			}
		}
		else {
			panic("unknown function : " + f);
		}

		m_game->m_used_funcs.insert(f);
		return Yisp::Void::get();
	}

	std::shared_ptr<Yisp::Object> Executor::execNothing(std::stringstream& s)
	{
		int cnt = 0;
		read_while(s, [&cnt](char c) {
			if (c == '(') {
				cnt++;
			}
			if (cnt == 0) {
				return false;
			}
			if (c == ')') {
				cnt--;
			}
			return true;
		});
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
			s.unget();
			t_string x = read_while(s, [](char c) {
				return c >= 'a' && c <= 'z';
			});
			int val = x.size() == 1 ? m_vars[x[0]] : m_game->m_vars[x];
			return make_shared<Yisp::Number>(val);
		}
		if (c == '|') {
			shared_ptr<Yisp::CardSet> card_set(execSet(s));
			remove_space(s);
			if (s.get() != '|') {
				panic("| not match in execNumber");
			}
			return make_shared<Yisp::Number>(card_set->size());
		}
		if (c == '(') {
			remove_space(s);
			auto op = read_while(s, [](char c) {
				return !isspace(c);
			});
			remove_space(s);
			int num1 = execNumber(s)->num;
			remove_space(s);
			int num2 = execNumber(s)->num;
			int res;
			if (op == "+") {
				res = num1 + num2;
			}
			else if (op == "-") {
				res = num1 - num2;
			}
			else if (op == ">") {
				res = num1 > num2;
			}
			else if (op == "<") {
				res = num1 < num2;
			}
			else if (op == "==") {
				res = num1 == num2;
			}
			else if (op == "and") {
				res = (num1 != 0) & (num2 != 0);
			}
			else if (op == "or") {
				res = (num1 != 0) | (num2 != 0);
			}
			else if (op == "r") {
				res = random_int(num1, num2);
			}
			else {
				panic("unknown operator [" + op + "] in " + s.str());
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
			return c >= 0 && m_set_allowed_chars[c];
		});
		vector<t_string> ws = split(w, ".");
		shared_ptr<CardCollection> collection = nullptr;

		if (m_game->m_name_to_collection.count(ws[0])) {
			collection = m_game->m_name_to_collection[ws[0]];
		}
		else if (ws[0] == "X") {
			shared_ptr<Yisp::CardSet> card_set(make_shared<Yisp::CardSet>());
			card_set->collection = m_game->find_card_position(m_card_it);
			card_set->cards_its.push_back(m_card_it);
			return card_set;
		}
		else {
			panic("unknown collection: " + ws[0]);
		}
		shared_ptr<Yisp::CardSet> card_set(make_shared<Yisp::CardSet>(collection));

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

	shared_ptr<Yisp::Number> Executor::execCondition(std::stringstream& s)
	{
		//start from next char after '/'
		char c;
		c = s.peek();
		if (c == '!') {
			s.get(c);
			auto res = execCondition(s)->num != 0;
			return make_shared<Yisp::Number>(!res);
		}
		return execNumber(s);
	}

	YGO::Executor::Executor(Game* game, shared_ptr<CardCollection> src, CardNode card_it, int opt)
		:m_game(game), m_src(src), m_card_it(card_it), m_opt(opt), m_cond_break(false),
			m_activated(false), m_vars(128), m_set_allowed_chars(256)
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
		m_set_allowed_chars['!'] = true;
	}

	bool YGO::Executor::run()
	{
		
		Card card = *m_card_it;
		vector<t_string> statements = split(card.m_effects[m_opt].m_program, ";");
		for (auto statement : statements) {
			//printf("---------%s-----------", statement.c_str());
			//printf("deck: %d, hand: %d\n", m_game->m_deck->size(), m_game->m_hand->size());
			//for (auto it = m_game->m_hand->begin(); it != m_game->m_hand->end(); ++it) {
			//	cout << it->print_name() << endl;
			//}

			trim(statement);
			if (statement.empty()) {
				continue;
			}
			stringstream ss(statement);
			execStatement(ss);
			if (m_cond_break) {
				break;
			}
		}
		return m_activated;
	}
	bool YGO::Executor::run_header(t_string header) {
		vector<t_string> statements = split(header, ";");
		for (auto statement : statements) {
			trim(statement);
			if (statement.empty()) {
				continue;
			}
			stringstream ss(statement);
			execStatement(ss);
			if (m_cond_break) {
				break;
			}
		}
		return true;
	}
}
