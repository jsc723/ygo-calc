#include "simulator.h"
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
using namespace std;

YGO::Simulator::Simulator(YAML::Node simulate)
{
	auto count_node = simulate["count"];
	m_count = count_node.IsDefined() ? count_node.as<int>() : 50000;
	auto separate_node = simulate["separate"];
	m_separate = separate_node.IsDefined() ? separate_node.as<bool>() : false;

	auto start_card_node = simulate["start_card"];
	m_start_card = start_card_node.IsDefined() ? start_card_node.as<int>() : 5;
	auto turns_node = simulate["turns"];
	m_turns = turns_node.IsDefined() ? turns_node.as<int>() : 5;

	auto tests = simulate["tests"];
	if (tests.IsDefined())
	{
		for (auto it = tests.begin(); it != tests.end(); ++it)
		{
			Topic topic;
			topic.name = it->first.as<t_string>();
			auto topic_node = it->second;
			for (auto jt = topic_node.begin(); jt != topic_node.end(); ++jt)
			{
				Combo combo;
				combo.name = jt->first.as<t_string>();
				auto combo_node = jt->second;
				if (combo_node.IsDefined() && combo_node.IsSequence())
				{
					for (int j = 0; j < combo_node.size(); j++)
					{
						t_string cond_k = combo_node[j].as<t_string>();
						combo.condition_strings.push_back(cond_k);
					}
				}
				topic.m_combos.emplace_back(std::move(combo));
			}
			m_topics.emplace_back(std::move(topic));
		}
	}
}

void YGO::Simulator::run(const Deck& deck, Context& context)
{
	for (int i = 0; i < m_topics.size(); i++)
	{
		m_topics[i].bind(context);
		m_topics[i].print(std::cout);
		cout << "\n";
	}
	cout << "\n";

	int num_topic = m_topics.size();
	vector<vector<vector<int>>> success(m_turns); //turn, topic, combo
	vector<vector<int>> total_success(m_turns, vector<int>(num_topic)); //turn, topic
	int max_num_combo = 0;
	for (int t = 0; t < m_turns; t++)
	{
		success[t] = vector<vector<int>>(num_topic);
		for (int i = 0; i < num_topic; i++)
		{
			int num_combo = m_topics[i].m_combos.size();
			success[t][i] = vector<int>(num_combo);
			max_num_combo = max(max_num_combo, num_combo);
		}
	}

	vector<Card> cards = deck.generate();
	if (m_start_card + m_turns > cards.size())
	{
		panic("simulate: start_card + turns must be smaller than deck size");
	}

	std::random_device rd;

	printf("Simulate %d times...\n", m_count);
	auto start_time = chrono::high_resolution_clock::now();
	vector<bool> combo_test(max_num_combo);

	for (int k = 0; k < m_count; k++)
	{
		std::shuffle(cards.begin(), cards.end(), rd);
		vector<Card> handCards(cards.begin(), cards.begin() + m_start_card);
		for (int t = 0; t < m_turns; t++)
		{
			for (int i = 0; i < m_topics.size(); i++)
			{
				const int num_combo = m_topics[i].m_combos.size();
				fill(combo_test.begin(), combo_test.begin() + num_combo, false);
				bool any_success = false;
				for (int j = 0; j < num_combo; j++)
				{
					combo_test[j] = m_topics[i].m_combos[j].test(handCards);
					any_success |= combo_test[j];
					success[t][i][j] += combo_test[j];
				}
				total_success[t][i] += any_success;
			}
			handCards.push_back(cards[m_start_card + t + 1]); //draw
		}
	}

	auto finish_time = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(finish_time - start_time);
	cout << "Time used: " << duration.count() << "ms\n";

	for (int i = 0; i < num_topic; i++)
	{
		cout << "Topic: " << m_topics[i].name << endl;
		for (int t = 0; t < m_turns; t++) {
			const int num_combo = m_topics[i].m_combos.size();
			printf("Turn %d total: %.2lf%%  ", t + 1, (double)total_success[t][i] / m_count * 100.0);
			for (int j = 0; j < num_combo; j++)
			{
				printf("%s: %.2lf%%  ", m_topics[i].m_combos[j].name.c_str(), (double)success[t][i][j] / m_count * 100.0);
			}
			printf("\n");
		}
		printf("\n");
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
