#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include "global.h"
namespace YGO {
	struct Effect
	{
		t_string m_prog_attribute;
		t_string m_valid_position;
		t_string m_program;
		bool exec_once_each_turn() const;
		bool exec_at_beginning() const;
		bool is_executable_at(t_string position) const;
	};

	class Card
	{
		t_string m_name;
		int m_count = 1;
		std::vector<t_string> m_attribute;
		t_string m_description;
		
	public:
		Card() = default;
		Card(const Card& other) = default;
		Card(const t_string name, const YAML::Node& node);
		Card& operator=(const Card &rhs) = default;
		friend std::ostream& operator<<(std::ostream& os, const Card& card);
		t_string name() const { return m_name; }
		t_string effect_name(int idx) const;
		t_string description() const { return m_description; }
		t_string print_name() const {
			return m_description.empty() ? m_name : m_description;
		}
		bool test_attribute(const t_string& attr) const;
		bool test_attribute_wildcard(const t_string& pattern) const;
		int count() const { return m_count; }
		void set_count(int cnt) { m_count = cnt; }
		bool exec_once_each_turn() const;
		bool exec_at_beginning() const;
		std::vector<Effect> m_effects;
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

