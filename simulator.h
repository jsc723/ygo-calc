#pragma once
#include <yaml-cpp/yaml.h>
#include <vector>
#include <iostream>
#include "deck.h"
#include "context.h"
#include "condition.h"

namespace YGO {
	using condition_set_t = std::set<YGO::Condition*, YGO::ConditionPtrCompare>;
	class Game;
	class Simulator
	{
		int m_count;
		bool m_separate;
		struct Combo {
			t_string name;
			double score = 1.0;
			std::vector<t_string> hand_condition_strings;
			std::vector<Condition*> hand_conditions;
			std::vector<t_string> grave_condition_strings;
			std::vector<Condition*> grave_conditions;
			bool test(const Game& g);
			void bind(Context& context);
			void print(std::ostream& os);
		};

	public:
		struct Topic {
			t_string name;
			std::vector<Combo> m_combos;
			int m_start_card;
			bool m_exec_program;
			t_string m_header;
			bool test(const Game& g);
			void bind(Context& context);
			void print(std::ostream& os);
			condition_set_t get_wanted_hand_conds() const;
			condition_set_t get_wanted_grave_conds() const;
		};
		std::vector<Topic> m_topics;
		Simulator(YAML::Node simulate);
		void run(const Deck& deck, Context& context);

	};
}

