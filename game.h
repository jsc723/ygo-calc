#pragma once
#include <sstream>
#include <memory>
#include <utility>
#include <vector>
#include "CardCollection.h"
#include "condition.h"

namespace YGO {
	
	class Game
	{
	public:
		Game(Deck &deck_template, int start_hand_cards, const std::vector<Condition*>& wanted_conds);
		bool execute_hand_card(int index, int opt);
		std::vector<int> select_add_hand_card(const std::vector<Card>& cards, int k);
		std::shared_ptr<CardCollection> m_deck;
		std::shared_ptr<CardCollection> m_hand;
		std::shared_ptr<CardCollection> m_field;
		std::shared_ptr<CardCollection> m_bochi;
		std::shared_ptr<CardCollection> m_jyogai;
		std::vector<Condition*> m_wanted_conds;
		std::vector<bool> m_forbidden_funcs;
	};

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
			std::vector<int> cards_idx;
			bool valid;
			CardSet() :valid(true) {}
			CardSet(std::shared_ptr<CardCollection> clt): valid(true), collection(clt) {
				for (int i = 0; i < clt->size(); i++) {
					cards_idx.emplace_back(i);
				}
			}
			const Card& operator[](int i) {
				if (!valid) panic("use invalid carset");
				return collection->get(cards_idx[i]);
			}
			size_t size() {
				if (!valid) panic("use invalid carset");
				return cards_idx.size();
			}
			void move_to_back(std::shared_ptr<CardCollection> dst);
			void move_to_back(std::shared_ptr<CardCollection> dst, const std::vector<int> &indexs);
			std::shared_ptr<CardSet> subset(int n);
			std::shared_ptr<CardSet> subset(t_string cond);
			std::vector<Card> get_cards();
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