#pragma once
#include <string>

class Preprocessor {
public:
	static std::string preprocess_includes(std::string filename);
};