#pragma once
#include <string>
#include <cstdlib>
#include <iostream>
namespace YGO
{
	using t_string = std::string;

	inline void panic(const t_string& msg, int code = 1) {
		std::cout << msg << std::endl;
		system("pause");
		exit(code);
	}
	bool wildCardMatch(const t_string& s, const t_string& p);
}
