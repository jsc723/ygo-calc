#pragma once
#include "CardCollection.h"
namespace YGO {
	
	class Game
	{
	public:
		Game(Deck &deck_template, int start_hand_cards);
		bool execute_hand_card(int index, int opt);
		CardCollection* m_deck;
		CardCollection* m_hand;
		CardCollection* m_field;
		CardCollection* m_bochi;
		CardCollection* m_jyogai;
	};

	namespace Yisp {
		struct Object {
		};
		struct Void : public Object {
		protected:
			Void() {
				magic = 13572468;
			}
			static Void* void_;
			int magic;
		public:
			Void(Void& other) = delete;
			void operator=(const Void&) = delete;
			static Void* get() { return void_; }
		};
		struct Number : public Object {
			int num;
		};
		struct CardSet : public Object {
			CardCollection* c;
		};
	}

	class Executor
	{
		Game* m_game;
		CardCollection* m_src;
		int m_card_index;
		int m_opt;
		Yisp::Object* execStatement(const t_string& s);
	public:
		Executor(Game* game, CardCollection* src, int card_index, int opt);
		bool run();
	};
}