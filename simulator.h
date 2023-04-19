#pragma once
#include <yaml-cpp/yaml.h>
#include <vector>
#include <iostream>
#include "deck.h"
#include "context.h"
#include "condition.h"
namespace YGO {
	class Simulator
	{
		int m_count;
		bool m_separate;
		int m_start_card;
		int m_turns;
		struct Combo {
			t_string name;
			std::vector<t_string> condition_strings;
			std::vector<Condition*> conditions;
			bool test(const std::vector<Card> cards);
			void bind(Context& context);
			void print(std::ostream& os);
		};
		struct Topic {
			t_string name;
			std::vector<Combo> m_combos;
			bool test(const std::vector<Card> cards);
			void bind(Context& context);
			void print(std::ostream& os);
		};
		std::vector<Topic> m_topics;
	public:
		Simulator(YAML::Node simulate);
		void run(const Deck& deck, Context& context);
	};
}

