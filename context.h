#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "global.h"
#include "deck.h"
#include "condition.h"
#include <ostream>
namespace YGO
{
	class Context
	{
		std::unordered_map<t_string, Condition*> m_context;
	public:
		Context() {}
		void addCardNameCondition(const Card& card)
		{
			m_context[card.name()] = new CardNameCondition(card.name());
		}
		void addCondition(const t_string& key, Condition* cond)
		{
			m_context[key] = cond;
		}
		Condition* getCondition(const t_string& key);
		void print(std::ostream& os) const
		{
			for (auto& [k, v] : m_context)
			{
				os << k << ": ";
				v->print(os);
				os << std::endl;
			}
		}
	};

	class Utils
	{
		static Condition* parseSingle(Context& context, const t_string& cond);
	public:
		static std::vector<t_string> strSplit(const t_string& s);
		static Condition* parse(Context& context, const t_string& cond);
	};

}
