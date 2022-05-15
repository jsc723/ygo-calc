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
			Combo combo;
			combo.name = it->first.as<t_string>();
			auto combo_node = it->second;
			if (combo_node.IsDefined() && combo_node.IsSequence()) 
			{
				for (int j = 0; j < combo_node.size(); j++)
				{
					t_string cond_k = combo_node[j].as<t_string>();
					combo.condition_names.push_back(cond_k);
				}
			}
			m_combos.emplace_back(std::move(combo));
		}
	}
}

void YGO::Simulator::run(const Deck& deck, Context& context)
{

	for (auto& combo : m_combos)
	{
		combo.bind(context);
		combo.print(std::cout);
		cout << "\n";
	}
	cout << "\n";

	vector<Card> cards = deck.generate();
	if (m_start_card + m_turns > cards.size())
	{
		panic("simulate: start_card + turns must be smaller than deck size");
	}
	std::random_device rd;
	const int combo_size = m_combos.size();

	vector<vector<int>> success(m_turns, vector<int>(combo_size));
	vector<int> total_success(m_turns);

	printf("Simulate %d times...\n", m_count);
	auto start_time = chrono::high_resolution_clock::now();

	for (int k = 0; k < m_count; k++)
	{
		std::shuffle(cards.begin(), cards.end(), rd);
		vector<Card> handCards(cards.begin(), cards.begin() + m_start_card);
		for (int t = 0; t < m_turns; t++)
		{
			if (m_separate)
			{
				vector<bool> res(combo_size);
				bool any_success = false;
				for (int i = 0; i < combo_size; i++)
				{
					res[i] = m_combos[i].test(handCards);
					any_success |= res[i];
					success[t][i] += res[i];
				}
				total_success[t] += any_success;
			}
			else
			{
				for (int i = 0; i < combo_size; i++)
				{
					if (m_combos[i].test(handCards))
					{
						total_success[t]++;
						break;
					}
				}
			}

			handCards.push_back(cards[m_start_card + t + 1]); //draw
		}
	}

	auto finish_time = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(finish_time - start_time);
	cout << "Time used: " << duration.count() << "ms\n";

	for (int t = 0; t < m_turns; t++) {
		printf("Turn %d total: %.2lf%%  ", t + 1, (double)total_success[t] / m_count * 100.0);
		if (m_separate)
		{
			for (int i = 0; i < combo_size; i++)
			{
				printf("%s: %.2lf%%  ", m_combos[i].name.c_str(), (double)success[t][i] / m_count * 100.0);
			}
		}
		printf("\n");
	}

}


void YGO::Simulator::Combo::bind(Context& context)
{
	conditions.clear();
	for (int i = 0; i < condition_names.size(); i++)
	{
		auto cond = context.getCondition(condition_names[i]);
		if (!cond) 
		{
			panic("simulate/" + name + ": cannot find condition " + condition_names[i]);
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
	for (auto cond : condition_names)
	{
		os << cond << "  ";
	}
}

