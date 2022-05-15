#include "global.h"

//ref: https://leetcode.com/problems/wildcard-matching/solution/
bool YGO::wildCardMatch(const t_string& s, const t_string& p)
{
	int sLen = s.size(), pLen = p.size();
	int sIdx = 0, pIdx = 0;
	int starIdx = -1, sTmpIdx = -1;

	while (sIdx < sLen)
	{
		if (pIdx < pLen && (p[pIdx] == '?' || p[pIdx] == s[sIdx]))
		{
			++sIdx;
			++pIdx;
		}
		else if (pIdx < pLen && p[pIdx] == '*')
		{
			starIdx = pIdx;
			sTmpIdx = sIdx;
			++pIdx;
		}
		else if (starIdx == -1)
		{
			return false;
		}
		else
		{
			pIdx = starIdx + 1;
			sIdx = sTmpIdx + 1;
			sTmpIdx = sIdx;
		}
	}

	for (int i = pIdx; i < pLen; i++)
	{
		if (p[i] != '*')
		{
			return false;
		}
	}
	return true;
}
