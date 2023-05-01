#include "deck.h"
#include <algorithm>
using namespace std;
namespace YGO
{
	Card::Card(const t_string name, const YAML::Node& node) : m_name(name)
	{
		auto& node_count = node["count"];
		m_count = node_count.IsDefined() ? node_count.as<int>() : 1;
		auto& attr = node["attribute"];
		if (attr.IsDefined() && attr.IsSequence())
		{
			for (int i = 0; i < attr.size(); i++)
			{
				m_attribute.emplace_back(attr[i].as<t_string>());
			}
			std::sort(m_attribute.begin(), m_attribute.end());
		}
		auto& description_node = node["description"];
		if (description_node.IsDefined()) {
			m_description = description_node.as<t_string>();
		}
		auto& program_node = node["program"];
		if (program_node.IsDefined()) {
			auto prog_str = program_node.as<t_string>();
			if (!prog_str.empty()) {
				if (prog_str[0] == '[') {
					int i = prog_str.find(']');
					if (i == -1) {
						panic("] not found");
					}
					m_prog_attribute = prog_str.substr(1, i - 1);
					prog_str = prog_str.substr(i + 1);
				}
				m_program = prog_str;
			}
		}
	}
	bool Card::test_attribute(const t_string& attr) const
	{
		return std::binary_search(m_attribute.begin(), m_attribute.end(), attr);
	}
	bool Card::test_attribute_wildcard(const t_string& pattern) const
	{
		return std::any_of(m_attribute.begin(), m_attribute.end(), [&](auto& attr) {
			return wildCardMatch(attr, pattern);
		});
	}
	std::ostream& operator<<(std::ostream& os, const Card& card)
	{
		os << "name: " << card.m_name << ", count: " << card.m_count;
		//if (card.m_description.size()) {
		//	os << ", description: " << card.m_description;
		//}
		if (card.m_attribute.size())
		{
			os << ", attribute: [";
			for (auto& attr : card.m_attribute)
			{
				os << attr << " ";
			}
			os << "]";
		}
		if (card.m_prog_attribute.size()) {
			os << ", prog_attr: [" << card.m_prog_attribute << "]";
		}
		if (card.m_program.size()) {
			os << ", program: " << card.m_program;
		}
		return os;
	}

	Deck::Deck(const YAML::Node& node)
	{
		auto size_node = node["size"];
		m_size =  size_node.IsDefined() ? size_node.as<int>() : 0;
		auto& cards = node["cards"];
		int auto_count_idx = -1;
		int specified_size = 0;
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			auto card_name = it->first.as<t_string>();
			auto& card_node = it->second;
			m_cards.emplace_back(card_name, card_node);
			if (m_cards.back().count() == -1) 
			{
				if (auto_count_idx != -1) 
				{
					cout << "can only have one count = -1" << endl;
					exit(1);
				}
				auto_count_idx = m_cards.size() - 1;
			}
			else 
			{
				specified_size += m_cards.back().count();
			}
		}
		if (auto_count_idx != -1) 
		{
			m_cards[auto_count_idx].set_count(m_size - specified_size);
		}
		else
		{
			m_size = specified_size;
		}
	}

	const std::vector<Card> Deck::generate() const
	{
		vector<Card> cards;
		for (auto& card : m_cards)
		{
			for (int i = 0; i < card.count(); i++)
			{
				cards.emplace_back(card);
				cards.back().set_count(1);
			}
		}
		return cards;
	}

	std::ostream& operator<<(std::ostream& os, const Deck& deck)
	{
		os << "size: " << deck.m_size << "\n";
		os << "cards: " << "\n";
		for (auto& card : deck.m_cards)
		{
			os << card << "\n";
		}
		os << endl;
		return os;
	}
}
