#pragma once
#include <string>
#include <iostream>
namespace YGO
{
	using t_string = std::string;

	inline void panic(const t_string& msg, int code = 1) {
		std::cout << msg << std::endl;
		exit(code);
	}
	bool wildCardMatch(const t_string& s, const t_string& p);
}
