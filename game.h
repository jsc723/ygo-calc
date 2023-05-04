#pragma once
#include <sstream>
#include <memory>
#include <utility>
#include <vector>
#include <unordered_set>
#include <set>
#include <queue>
#include "CardCollection.h"
#include "condition.h"
#include "simulator.h"

namespace YGO {

	namespace Yisp {
		struct Object {
		protected:
			virtual void foo() {
				std::cout << "hello" << std::endl;
			}
		};

		struct Void : public Object {
		protected:
			Void() {
				magic = 13572468;
			}
			static Void void_;
			int magic;
		public:
			Void(Void& other) = delete;
			void operator=(const Void&) = delete;
			static std::shared_ptr<Void> get() {
				static std::shared_ptr<Void> ptr(&void_);
				return ptr;
			}
		};

		struct Number : public Object {
			int num;
			Number(int n) : num(n) {}
		};

		struct CardSet : public Object {
			std::shared_ptr<CardCollection> collection;
			std::list<CardNode> cards_its;
			CardSet() {}
			CardSet(std::shared_ptr<CardCollection> clt) : collection(clt) {
				for (auto it = clt->begin(); it != clt->end(); it++) {
					cards_its.push_back(it);
				}
			}
			size_t size() {
				return cards_its.size();
			}
			void move_to_back(std::shared_ptr<CardCollection> dst);
			std::shared_ptr<CardSet> subset(int n);
			std::shared_ptr<CardSet> subset(t_string cond);
		};

		struct String : public Object {
			t_string s;
			String(const t_string& s) : s(s) {}
		};
	}

	std::ostream& operator<<(std::ostream& os, const Yisp::CardSet& c);

	class Game
	{
		std::unordered_set<t_string> m_already_executed;
	public:
		Game(const Deck &deck_template, const Simulator::Topic &topic);
		void run();
		bool execute_card(std::shared_ptr<CardCollection> col, CardNode it, int opt);
		void select_add_hand_card(Yisp::CardSet& to_select, int k, std::shared_ptr<CardCollection> &dst);
		std::shared_ptr<CardCollection> find_card_position(CardNode it);
		t_string m_header;
		std::shared_ptr<CardCollection> m_deck;
		std::shared_ptr<CardCollection> m_hand;
		std::shared_ptr<CardCollection> m_field;
		std::shared_ptr<CardCollection> m_bochi;
		std::shared_ptr<CardCollection> m_jyogai;
		std::unordered_map<t_string, std::shared_ptr<CardCollection>> m_name_to_collection;
		YGO::condition_set_t m_wanted_hand_conds;
		YGO::condition_set_t m_wanted_grave_conds;
		std::unordered_set<t_string> m_forbidden_funcs;
		std::unordered_set<t_string> m_used_funcs;
		std::unordered_map<t_string, int> m_vars;

		struct MoveEvent {
			std::shared_ptr<CardCollection> src;
			std::shared_ptr<CardCollection> dst;
			std::vector<CardNode> moved;
			t_string way;
		};
		void inform_card_moved(const MoveEvent &e);
	};

	

	class Executor
	{
		Game* m_game;
		std::shared_ptr<CardCollection> m_src;
		CardNode m_card_it;
		int m_opt;
		bool m_cond_break;
		bool m_activated;
		std::vector<int> m_vars;
		std::vector<bool> m_set_allowed_chars;

		using parser_t = std::function<std::shared_ptr<Yisp::Object>(Executor*, std::stringstream&)>;
		
		std::shared_ptr<Yisp::Object> execStatement(std::stringstream& s);
		std::shared_ptr<Yisp::Object> execExpr(std::stringstream& s);
		std::vector< std::shared_ptr<Yisp::Object> > parseParams(std::stringstream& s, std::vector<parser_t> parserFuncs);
		std::shared_ptr<Yisp::Object> execFunc(std::stringstream& s);
		std::shared_ptr<Yisp::Number> execNumber(std::stringstream& s);
		std::shared_ptr<Yisp::CardSet> execSet(std::stringstream& s);
		std::shared_ptr<Yisp::String> execString(std::stringstream& s);
		std::shared_ptr<Yisp::Number> execCondition(std::stringstream& s);
		std::shared_ptr<Yisp::Object> execNothing(std::stringstream& s);
	public:
		Executor(Game* game, std::shared_ptr<CardCollection> src, CardNode card_it, int opt);
		bool run();
		bool run_header(t_string header);
	};
}