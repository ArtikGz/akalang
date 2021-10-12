#pragma once
#include <string>

class Utils{
public:
	static std::string read_file(std::string filepath);
	static void error(std::string message...);
};