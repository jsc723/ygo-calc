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
		int m_count = 1;
		std::vector<t_string> m_attribute;
		t_string m_description;
		t_string m_prog_attribute;
		t_string m_program;
	public:
		Card() = default;
		Card(const Card& other) = default;
		Card(const t_string name, const YAML::Node& node);
		Card& operator=(const Card &rhs) = default;
		friend std::ostream& operator<<(std::ostream& os, const Card& card);
		t_string name() const { return m_name; }
		bool test_attribute(const t_string& attr) const;
		bool test_attribute_wildcard(const t_string& pattern) const;
		int count() const { return m_count; }
		void set_count(int cnt) { m_count = cnt; }
		t_string prog_attribute() { return m_prog_attribute; }
		t_string program() { return m_program; }
		bool is_executable() const noexcept { return !m_program.empty(); }
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
		int size() const { return m_size; }
		const std::vector<Card> generate() const;
	};
	std::ostream& operator<<(std::ostream& os, const Deck& deck);


}

