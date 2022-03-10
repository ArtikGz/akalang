#pragma once
#include <string>
#include "lexer.hpp"

class Preprocessor {
public:
	static void preprocess_includes(const std::string& filename, std::vector<std::string>& filenames, std::vector<Token>& tokens);
};