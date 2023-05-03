#pragma once
#include "global.h"
#include "deck.h"
#include <vector>
#include <ostream>
namespace YGO
{
	class Condition
	{
	public:
		virtual bool match(const Card& card) const = 0;
		virtual void print(std::ostream& os) const = 0;
		virtual bool operator==(const Condition& other) const = 0;
		virtual bool operator!=(const Condition& other) const {
			return !(*this == other);
		}
		virtual bool operator<(const Condition& other) const {
			return this < &other;
		}
	};

	class ConditionPtrCompare {
	public:
		bool operator()(const Condition* a, const Condition* b) const {
			if (*a == *b) {
				return false;
			}
			return *a < *b;
		}
	};

	class CardNameCondition : public Condition
	{
		const t_string m_name;
	public:
		CardNameCondition(const t_string &name): m_name(name) {}
		bool match(const Card& card) const override
		{
			return m_name == card.name();
		}
		void print(std::ostream& os) const override
		{
			os << "name==" << m_name;
		}
		bool operator==(const Condition& other) const override
		{
			if (const CardNameCondition* c = dynamic_cast<const CardNameCondition*>(&other)) {
				return m_name == c->m_name;
			}
			return false;
		}
	};

	class CardAttributeCondition : public Condition
	{
		const t_string m_attr;
	public:
		CardAttributeCondition(const t_string attr): m_attr(attr) {}
		bool match(const Card& card) const override
		{
			return card.test_attribute(m_attr);
		}
		void print(std::ostream& os) const override
		{
			os << "attr=" << m_attr;
		}
		bool operator==(const Condition& other) const override
		{
			if (const CardAttributeCondition* c = dynamic_cast<const CardAttributeCondition*>(&other)) {
				return m_attr == c->m_attr;
			}
			return false;
		}
	};

	class CardAttributeWildcardCondition : public Condition
	{
		const t_string m_pattern;
	public:
		CardAttributeWildcardCondition(const t_string pattern) : m_pattern(pattern) {}
		bool match(const Card& card) const override
		{
			return card.test_attribute_wildcard(m_pattern);
		}
		void print(std::ostream& os) const override
		{
			os << "attr~" << m_pattern;
		}
		bool operator==(const Condition& other) const override
		{
			if (const CardAttributeWildcardCondition* c = dynamic_cast<const CardAttributeWildcardCondition*>(&other)) {
				return m_pattern == c->m_pattern;
			}
			return false;
		}
	};

	class NotCondition : public Condition
	{
		const Condition* child;
	public:
		NotCondition(Condition* condition) : child(condition) {}
		bool match(const Card& card) const override
		{
			return !child->match(card);
		}
		void print(std::ostream& os) const override
		{
			os << "!";
			child->print(os);
		}
		bool operator==(const Condition& other) const override
		{
			if (const NotCondition* c = dynamic_cast<const NotCondition*>(&other)) {
				return *child == *(c->child);
			}
			return false;
		}
	};

	class AndCondition: public Condition
	{
		std::vector<Condition*> children;
	public:
		AndCondition() {}
		void addChild(Condition* condition)
		{
			children.push_back(condition);
		}
		bool match(const Card& card) const override
		{
			for (auto child : children)
			{
				if (!child->match(card))
				{
					return false;
				}
			}
			return true;
		}
		void print(std::ostream& os) const override
		{
			for (int i = 0; i + 1 < children.size(); i++)
			{
				children[i]->print(os);
				os << " && ";
			}
			children.back()->print(os);
		}
		bool operator==(const Condition& other) const override
		{
			if (const AndCondition* c = dynamic_cast<const AndCondition*>(&other)) {
				if (children.size() != c->children.size()) {
					return false;
				}
				for (int i = 0; i < children.size(); i++) {
					if (*children[i] != *(c->children[i])) {
						return false;
					}
				}
				return true;
			}
			return false;
		}
	};

}

