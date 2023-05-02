#pragma once
#include <sstream>
#include <memory>
#include <utility>
#include <unordered_set>
#include "CardCollection.h"
#include "condition.h"

namespace YGO {
	
	class Game
	{
	public:
		Game(Deck &deck_template, int start_hand_cards, const std::unordered_set<Condition*>& good_conds);
		bool execute_hand_card(int index, int opt);
		std::shared_ptr<CardCollection> m_deck;
		std::shared_ptr<CardCollection> m_hand;
		std::shared_ptr<CardCollection> m_field;
		std::shared_ptr<CardCollection> m_bochi;
		std::shared_ptr<CardCollection> m_jyogai;
		std::unordered_set<Condition*> m_good_conds;
	};

	namespace Yisp {
		struct Object {
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
			std::vector<int> cards;
			CardSet() {}
			CardSet(std::shared_ptr<CardCollection> clt): collection(clt) {
				for (int i = 0; i < clt->size(); i++) {
					cards.emplace_back(i);
				}
			}
			const Card& operator[](int i) {
				return collection->get(cards[i]);
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

	class Executor
	{
		Game* m_game;
		std::shared_ptr<CardCollection> m_src;
		int m_card_index;
		int m_opt;
		bool m_cond_break;
		std::vector<int> m_vars;
		std::vector<bool> m_set_allowed_chars;
		std::vector<bool> m_forbidden_funcs;
		std::shared_ptr<Yisp::Object> execStatement(std::stringstream& s);
		std::shared_ptr<Yisp::Object> execExpr(std::stringstream& s);
		std::shared_ptr<Yisp::Object> execFunc(std::stringstream& s);
		std::shared_ptr<Yisp::Number> execNumber(std::stringstream& s);
		std::shared_ptr<Yisp::CardSet> execSet(std::stringstream& s);
		std::shared_ptr<Yisp::Object> execCondition(std::stringstream& s);
	public:
		Executor(Game* game, std::shared_ptr<CardCollection> src, int card_index, int opt);
		bool run();
	};
}