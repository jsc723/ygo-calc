#include "global.h"
#include <random>
#include <Windows.h>

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

double YGO::compute_ci(double p, int n) {
	return 1.96 * sqrt(p * (1 - p) / n);
}
double YGO::compute_mean(const std::vector<double> data) {
	if (data.empty()) {
		return 0;
	}
	double sum = 0;
	for (auto x : data) {
		sum += x;
	}
	return sum / data.size();
}
double YGO::compute_std(const std::vector<double> data) {
	if (data.size() <= 1) {
		return 0;
	}
	double mean = compute_mean(data);
	double sum = 0;
	int n = data.size();
	for (auto x : data) {
		sum += (x - mean) * (x - mean);
	}
	return sqrt(sum / (n - 1));
}

std::string YGO::utf8_to_local_encoding(const std::string& utf8String) {

	// Calculate the required buffer size for the converted string
	int requiredSize = MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, nullptr, 0);

	// Allocate a wide-character buffer
	std::wstring wideString(requiredSize, 0);

	// Convert the UTF-8 string to wide-character
	MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, &wideString[0], requiredSize);

	// Calculate the required buffer size for the console encoding
	int consoleSize = WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, nullptr, 0, nullptr, nullptr);

	// Allocate a buffer for the console-encoded string
	std::string consoleString(consoleSize, 0);

	// Convert the wide-character string to the console encoding
	WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, &consoleString[0], consoleSize, nullptr, nullptr);

	return consoleString;
}

YGO::UTF8CodePage::UTF8CodePage() : m_old_code_page(::GetConsoleOutputCP()) {
	::SetConsoleOutputCP(CP_UTF8);
}

YGO::UTF8CodePage::~UTF8CodePage()
{
	::SetConsoleOutputCP(m_old_code_page);
}
