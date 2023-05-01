#pragma once
#include <string>
#include <cstdlib>
#include <iostream>
#include <vector>
namespace YGO
{
	using t_string = std::string;

	inline void panic(const t_string& msg, int code = 1) {
		std::cout << msg << std::endl;
		system("pause");
		exit(code);
	}
	bool wildCardMatch(const t_string& s, const t_string& p);
	std::vector<std::string> split(std::string s, std::string delimiter);

	void ltrim(std::string& s);
	void rtrim(std::string& s);
	void trim(std::string& s);

}
