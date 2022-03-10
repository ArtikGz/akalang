#pragma once
#include <string>
#include "token.hpp"

class Utils{
public:
	static std::string read_file(const std::string& filepath);
	static void error(const std::string& message);
	static void error(const std::string& message, TokenLoc token);
	static bool is_blankspace(char c);
};
