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
	};

	class CardAttributeCondition : public Condition
	{
		const t_string m_attr;
	public:
		CardAttributeCondition(const t_string attr): m_attr(attr) {}
		bool match(const Card& card) const override
		{
			return card.testAttribute(m_attr);
		}
		void print(std::ostream& os) const override
		{
			os << "attr=" << m_attr;
		}
	};

	class CardAttributeWildcardCondition : public Condition
	{
		const t_string m_pattern;
	public:
		CardAttributeWildcardCondition(const t_string pattern) : m_pattern(pattern) {}
		bool match(const Card& card) const override
		{
			return card.testAttributeWildcard(m_pattern);
		}
		void print(std::ostream& os) const override
		{
			os << "attr~" << m_pattern;
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
	};

}

