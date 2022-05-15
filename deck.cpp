#include "deck.h"
using namespace std;
namespace YGO
{
	Card::Card()
	{
	}
	Card::Card(const Card& other)
		:m_name(other.m_name), m_count(other.m_count),
		m_attribute(other.m_attribute)
	{
	}
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
		}
	}
	Card Card::operator=(const Card& rhs)
	{
		m_name = rhs.m_name;
		m_count = rhs.m_count;
		m_attribute = rhs.m_attribute;
		return *this;
	}
	std::ostream& operator<<(std::ostream& os, const Card& card)
	{
		os << "name: " << card.m_name << ", count: " << card.m_count;
		if (card.m_attribute.size())
		{
			os << ", attribute: [";
			for (auto& attr : card.m_attribute)
			{
				os << attr << " ";
			}
			os << "]";
		}
		return os;
	}

	Deck::Deck(const YAML::Node& node)
	{
		m_size = node["size"].as<int>();
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
