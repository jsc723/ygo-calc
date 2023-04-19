
#include "context.h"
#include <sstream>
using namespace std;
namespace YGO
{
	Condition* Utils::parseSingle(Context& context, const t_string& cond)
	{
		Condition* res = nullptr;
		if ((res = context.getCondition(cond)) != nullptr)
		{
			return res;
		}
		if (cond.size() > 1 && cond[0] == '!')
		{
			auto child = parseSingle(context, cond.substr(1));
			return new NotCondition(child);
		}
		if (cond.size() > 2 && cond[1] == ':')
		{
			switch (cond[0])
			{
			case 'a':
				return new CardAttributeCondition(cond.substr(2));
			case 'A':
				return new CardAttributeWildcardCondition(cond.substr(2));
			}
		}
		
		panic("Cannot parse condition: " + cond);
	}
	std::vector<t_string> Utils::strSplit(const t_string& s)
	{
		std::istringstream iss(s);
		return vector<t_string>(istream_iterator<t_string>(iss), istream_iterator<t_string>());
	}

	Condition* Utils::parse(Context& context, const t_string& cond)
	{
		vector<t_string> conds = strSplit(cond);
		if (conds.size() == 0)
		{
			panic("condition cannot be empty");
		}
		auto andCond = new AndCondition();
		for (auto& c : conds)
		{
			andCond->addChild(parseSingle(context, c));
		}
		return andCond;
	}

	Condition* Context::getCondition(const t_string& key)
	{
		if (m_context.count(key))
		{
			return m_context[key];
		}
		return nullptr;
	}

}
