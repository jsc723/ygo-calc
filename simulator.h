#pragma once
#include <yaml-cpp/yaml.h>
#include <vector>
#include <iostream>
#include "deck.h"
#include "context.h"
#include "condition.h"
namespace YGO {
	using condition_set_t = std::set<YGO::Condition*, YGO::ConditionPtrCompare>;
	class Simulator
	{
		int m_count;
		bool m_separate;
		struct Combo {
			t_string name;
			double score = 1.0;
			std::vector<t_string> condition_strings;
			std::vector<Condition*> conditions;
			bool test(const std::vector<Card> cards);
			void bind(Context& context);
			void print(std::ostream& os);
		};
		struct Topic {
			t_string name;
			std::vector<Combo> m_combos;
			int m_start_card;
			bool m_exec_program;
			bool test(const std::vector<Card> cards);
			void bind(Context& context);
			void print(std::ostream& os);
			condition_set_t get_wanted_conds();
		};
		std::vector<Topic> m_topics;
	public:
		Simulator(YAML::Node simulate);
		void run(const Deck& deck, Context& context);
	};
}

