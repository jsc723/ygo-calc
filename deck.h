#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include "global.h"
namespace YGO {

	class Card
	{
		t_string m_name;
		int m_count;
		std::vector<t_string> m_attribute;
	public:
		Card();
		Card(const Card& other);
		Card(const t_string name, const YAML::Node& node);
		Card operator=(const Card &rhs);
		friend std::ostream& operator<<(std::ostream& os, const Card& card);
		t_string name() const { return m_name; }
		bool testAttribute(const t_string& attr) const;
		bool testAttributeWildcard(const t_string& pattern) const;
		int count() const { return m_count; }
		void set_count(int cnt) { m_count = cnt; }
	};
	std::ostream& operator<<(std::ostream& os, const Card& card);

	class Deck
	{
		int m_size;
		std::vector<Card> m_cards;
	public:
		friend std::ostream& operator<<(std::ostream& os, const Deck& deck);
		Deck(const YAML::Node& node);
		const std::vector<Card>& cards() const
		{
			return m_cards;
		}
		const std::vector<Card> generate() const;
	};
	std::ostream& operator<<(std::ostream& os, const Deck& deck);


}

