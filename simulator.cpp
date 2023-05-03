#include "simulator.h"
#include "game.h"
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <set>

using namespace std;

YGO::condition_set_t YGO::Simulator::Topic::get_wanted_conds() {
	condition_set_t res_set;
	for (auto &combo : m_combos) {
		for (auto p_cond : combo.conditions) {
			res_set.emplace(p_cond);
		}
	}
	return res_set;
}

YGO::Simulator::Simulator(YAML::Node simulate)
{
	auto count_node = simulate["count"];
	m_count = count_node.IsDefined() ? count_node.as<int>() : 1000;
	auto separate_node = simulate["separate"];
	m_separate = separate_node.IsDefined() ? separate_node.as<bool>() : false;

	auto tests = simulate["tests"];
	if (tests.IsDefined())
	{
		for (auto it = tests.begin(); it != tests.end(); ++it)
		{
			Topic topic;
			topic.name = it->first.as<t_string>();
			auto topic_node = it->second;

			auto start_card_node = topic_node["start-card"];
			topic.m_start_card = start_card_node.IsDefined() ? start_card_node.as<int>() : 5;

			auto exec_program_node = topic_node["exec-program"];
			topic.m_exec_program = exec_program_node.IsDefined() ? exec_program_node.as<bool>() : false;

			auto topic_combos_node = topic_node["combos"];
			for (auto jt = topic_combos_node.begin(); jt != topic_combos_node.end(); ++jt)
			{
				Combo combo;
				combo.name = jt->first.as<t_string>();
				auto combo_node = jt->second;
				if (combo_node["score"].IsDefined()) {
					combo.score = combo_node["score"].as<double>();
				}
				auto combo_conds_node = combo_node["conditions"];
				if (combo_conds_node.IsDefined() && combo_conds_node.IsSequence())
				{
					for (int j = 0; j < combo_conds_node.size(); j++)
					{
						t_string cond_k = combo_conds_node[j].as<t_string>();
						combo.condition_strings.push_back(cond_k);
					}
				}
				else {
					panic("Combo is not correctly defined: " + combo.name);
				}
				topic.m_combos.emplace_back(std::move(combo));
			}
			m_topics.emplace_back(std::move(topic));
		}
	}
}

void YGO::Simulator::run(const Deck& deck_template, Context& context)
{
	for (int i = 0; i < m_topics.size(); i++)
	{
		m_topics[i].bind(context);
		m_topics[i].print(std::cout);
		cout << "\n";
	}
	cout << "\n";

	int num_topic = m_topics.size();
	vector<vector<int>> success(num_topic); //topic, combo
	vector<int> total_success(num_topic); //topic
	vector<double> total_score(num_topic); //topic
	int max_num_combo = 0;
	for (int i = 0; i < num_topic; i++)
	{
		int num_combo = m_topics[i].m_combos.size();
		success[i] = vector<int>(num_combo);
		max_num_combo = max(max_num_combo, num_combo);
	}

	std::random_device rd;

	printf("Simulate %d times...\n", m_count);
	auto start_time = chrono::high_resolution_clock::now();

	for (int k = 0; k < m_count; k++)
	{
		
		for (int i = 0; i < m_topics.size(); i++)
		{
			Game g(deck_template, m_topics[i].m_start_card, m_topics[i].get_wanted_conds());
			if (m_topics[i].m_exec_program) {
				g.run();
			}
			auto handCards = g.m_hand->get_all();
			const int num_combo = m_topics[i].m_combos.size();

			bool any_success = false;
			double max_topic_score = 0.0;
			for (int j = 0; j < num_combo; j++)
			{
				if (m_topics[i].m_combos[j].test(handCards)) {
					success[i][j]++;
					any_success = true;
					max_topic_score = std::max(max_topic_score, m_topics[i].m_combos[j].score);
				}
			}
			total_success[i] += any_success;
			total_score[i] += max_topic_score;
		}
	}

	auto finish_time = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(finish_time - start_time);
	std::cout << "Time used: " << duration.count() << "ms\n";

	for (int i = 0; i < num_topic; i++)
	{
		std::cout << "Topic: " << m_topics[i].name << endl;
		const int num_combo = m_topics[i].m_combos.size();
		printf("First turn average success rate: %.2lf%%    ", (double)total_success[i] / m_count * 100.0);
		printf("average score: %.2lf\n", (double)total_score[i] / m_count);
		for (int j = 0; j < num_combo; j++)
		{
			printf("    %s: %.2lf%%  ", m_topics[i].m_combos[j].name.c_str(), (double)success[i][j] / m_count * 100.0);
		}
		printf("\n\n");
	}

}

void YGO::Simulator::Combo::bind(Context& context)
{
	conditions.clear();
	for (auto condition_str: condition_strings)
	{
		auto *cond = context.getCondition(condition_str);
		if (!cond)
		{
			cond = Utils::parse(context, condition_str);
		}
		conditions.push_back(cond);
	}
}

bool YGO::Simulator::Combo::test(std::vector<Card> cards)
{
	vector<bool> used(cards.size());
	for (auto cond : conditions)
	{
		int found = false;
		for (int i = 0; i < cards.size(); i++)
		{
			if (!used[i] && cond->match(cards[i]))
			{
				used[i] = true;
				found = true;
				break;
			}
		}
		if (!found)
		{
			return false;
		}
	}
	return true;
}

void YGO::Simulator::Combo::print(ostream& os)
{
	os << name << ": ";
	for (auto cond : condition_strings)
	{
		os << cond << "  ";
	}
}
bool YGO::Simulator::Topic::test(std::vector<Card> cards)
{
	for (auto& comb : m_combos)
	{
		if (comb.test(cards))
		{
			return true;
		}
	}
	return false;
}
void YGO::Simulator::Topic::bind(Context& context)
{
	for (auto& comb : m_combos)
	{
		comb.bind(context);
	}
}

void YGO::Simulator::Topic::print(std::ostream& os)
{
	os << name << ": ";
	for (auto& comb : m_combos)
	{
		comb.print(os);
		os << "\n";
	}
}
