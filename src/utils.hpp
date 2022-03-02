#pragma once
#include <string>

class Utils{
public:
	static std::string read_file(const std::string& filepath);
	static void error(const std::string& message, ...);
	static bool is_blankspace(char c);
};
