#include "simulator.h"
#include "game.h"
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <set>

using namespace std;

YGO::condition_set_t YGO::Simulator::Topic::get_wanted_hand_conds() const {
	condition_set_t res_set;
	for (auto &combo : m_combos) {
		for (auto p_cond : combo.hand_conditions) {
			res_set.emplace(p_cond);
		}
	}
	return res_set;
}

YGO::condition_set_t YGO::Simulator::Topic::get_wanted_grave_conds() const {
	condition_set_t res_set;
	for (auto& combo : m_combos) {
		for (auto p_cond : combo.grave_conditions) {
			res_set.emplace(p_cond);
		}
	}
	return res_set;
}

YGO::Simulator::Simulator(YAML::Node simulate)
{
	auto count_node = simulate["count"];
	m_count = count_node.IsDefined() ? count_node.as<int>() : 1000;
	auto debug_node = simulate["debug"];
	m_debug = debug_node.IsDefined() ? debug_node.as<bool>() : false;
	auto CI_node = simulate["confidence-interval"];
	m_show_ci = CI_node.IsDefined() ? CI_node.as<bool>() : false;

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

			auto header_node = topic_node["header"];
			topic.m_header = header_node.IsDefined() ? header_node.as<string>() : "";

			auto topic_combos_node = topic_node["combos"];
			for (auto jt = topic_combos_node.begin(); jt != topic_combos_node.end(); ++jt)
			{
				Combo combo;
				combo.name = jt->first.as<t_string>();
				auto combo_node = jt->second;

				if (combo_node["score"].IsDefined()) {
					combo.score = combo_node["score"].as<string>();
				}

				if (combo_node["condition"].IsDefined()) {
					combo.condition = combo_node["condition"].as<string>();
				}

				auto combo_hand_node = combo_node["hand"];
				if (combo_hand_node.IsDefined() && combo_hand_node.IsSequence())
				{
					for (int j = 0; j < combo_hand_node.size(); j++)
					{
						t_string cond_k = combo_hand_node[j].as<t_string>();
						combo.hand_condition_strings.push_back(cond_k);
					}
				}

				auto combo_grave_node = combo_node["grave"];
				if (combo_grave_node.IsDefined() && combo_grave_node.IsSequence())
				{
					for (int j = 0; j < combo_grave_node.size(); j++)
					{
						t_string cond_k = combo_grave_node[j].as<t_string>();
						combo.grave_condition_strings.push_back(cond_k);
					}
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
	vector<vector<double>> topic_scores(num_topic, vector<double>(m_count)); //topic, scores
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
			Game g(deck_template, m_topics[i], m_debug);
			if (m_topics[i].m_exec_program) {
				g.run();
			}
			const int num_combo = m_topics[i].m_combos.size();

			bool any_success = false;
			int max_topic_score = 0;
			for (int j = 0; j < num_combo; j++)
			{
				int combo_score = m_topics[i].m_combos[j].test(g);
				if (combo_score > 0) {
					success[i][j]++;
					any_success = true;
					max_topic_score = std::max(max_topic_score, combo_score);
				}
			}
			total_success[i] += any_success;
			topic_scores[i][k] = max_topic_score;
		}
	}

	auto finish_time = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(finish_time - start_time);
	std::cout << "Time used: " << duration.count() << "ms\n";

	for (int i = 0; i < num_topic; i++)
	{
		std::cout << "Topic: " << m_topics[i].name << endl;
		const int num_combo = m_topics[i].m_combos.size();
		double p_success = (double)total_success[i] / m_count;
		printf("First turn average success rate: %.2lf%%", p_success * 100.0);
		if (m_show_ci) {
			printf(" +- %.2lf%%", compute_ci(p_success, m_count) * 100);
		}
		printf("  ");

		double mean_score = compute_mean(topic_scores[i]);
		
		printf("average score: %.2lf", mean_score);
		if (m_show_ci) {
			double std_score = compute_std(topic_scores[i]);
			printf(" +- %.2lf", 1.96 * (std_score / sqrt(m_count)));
		}
		printf("\n");
		for (int j = 0; j < num_combo; j++)
		{
			double p = (double)success[i][j] / m_count;
			printf("    %s: %.2lf%%", m_topics[i].m_combos[j].name.c_str(), p * 100.0);
			if (m_show_ci) {
				printf(" +- %.2lf%%", compute_ci(p, m_count) * 100);
			}
			printf("  ");
		}
		printf("\n\n");
	}

}

void YGO::Simulator::Combo::bind(Context& context)
{
	hand_conditions.clear();
	for (auto condition_str: hand_condition_strings)
	{
		auto *cond = context.getCondition(condition_str);
		if (!cond)
		{
			cond = Utils::parse(context, condition_str);
		}
		hand_conditions.push_back(cond);
	}

	grave_conditions.clear();
	for (auto condition_str : grave_condition_strings)
	{
		auto* cond = context.getCondition(condition_str);
		if (!cond)
		{
			cond = Utils::parse(context, condition_str);
		}
		grave_conditions.push_back(cond);
	}
}

int YGO::Simulator::Combo::test(Game &g)
{
	if (condition.size() && g.compute_number(condition) == 0) {
		return 0;
	}
	auto hand_cards = g.m_collections["H"]->to_vector();
	vector<bool> used(hand_cards.size());
	for (auto cond : hand_conditions)
	{
		int found = false;
		for (int i = 0; i < hand_cards.size(); i++)
		{
			if (!used[i] && cond->match(hand_cards[i]))
			{
				used[i] = true;
				found = true;
				break;
			}
		}
		if (!found)
		{
			return 0;
		}
	}

	auto grave_cards = g.m_collections["B"]->to_vector();
	used = vector<bool>(grave_cards.size());
	for (auto cond : grave_conditions)
	{
		int found = false;
		for (int i = 0; i < grave_cards.size(); i++)
		{
			if (!used[i] && cond->match(grave_cards[i]))
			{
				used[i] = true;
				found = true;
				break;
			}
		}
		if (!found)
		{
			return 0;
		}
	}

	return g.compute_number(score);
}

void YGO::Simulator::Combo::print(ostream& os)
{
	os << name << ": score: " << score << "; ";
	if (hand_condition_strings.size()) {
		os << "hand: ";
		for (auto cond : hand_condition_strings)
		{
			os << cond << "  ";
		}
	}
	if (grave_condition_strings.size()) {
		os << "grave: ";
		for (auto cond : grave_condition_strings)
		{
			os << cond << "  ";
		}
	}
}
int YGO::Simulator::Topic::test(Game& g)
{
	int max_score = 0;
	for (auto& comb : m_combos)
	{
		max_score = max(max_score, comb.test(g));
	}
	return max_score;
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
	os << name << ": " << endl;
	for (auto& comb : m_combos)
	{
		comb.print(os);
		os << "\n";
	}
}
