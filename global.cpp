#include "global.h"
#include <random>

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

//https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
std::vector<std::string> YGO::split(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

//https://stackoverflow.com/questions/216823/how-to-trim-an-stdstring
// trim from start (in place)
void YGO::ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
void YGO::rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
void YGO::trim(std::string& s) {
	rtrim(s);
	ltrim(s);
}

void YGO::remove_space(std::stringstream& ss) {
	while (std::isspace(ss.peek())) {
		ss.ignore();
	}
}

std::string YGO::read_while(std::stringstream& s, std::function<bool(char c)> pred) {
	std::string w;
	while (true) {
		char c = s.peek();
		if (c == std::char_traits<char>::eof() || !pred(c)) {
			break;
		}
		w += c;
		s.get();
	}
	return w;
}

int YGO::random_int(int low, int high) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(low, high);
	return dis(gen);
}