#pragma once
#include <string>

class Preprocessor {
public:
	static std::string preprocess_includes(const std::string& filename);
};