#include <iostream>
#include <vector>
#include <sstream>
#include "preprocessor.hpp"
#include "utils.hpp"
#include "lexer.hpp"
#include "token.hpp"

std::string Preprocessor::preprocess_includes(const std::string& filename, std::vector<std::string>& global_filenames) {
	std::stringstream fstring;

	Lexer lex(filename);
	while (lex.next_token().get_type() == Token::Type::INCLUDE_DIRECTIVE) {
		std::string name = lex.expect_next_token(Token::Type::LITERAL_STRING, "Preprocessing error: expected string after include").get_value();
		lex.set_file_content(lex.get_file_content().erase(0, lex.get_file_content().find_first_of("\n") + 1));
		lex.expect_next_token(Token::Type::SEMICOLON, "Preprocessing error: expected semicolon after include directive");

		bool skip = false;
		for (auto n: global_filenames) {
			if (n == name) {
				skip = true;
				break;
			}
		}

		if (!skip) {
			global_filenames.push_back(name);
			fstring << preprocess_includes(name, global_filenames) << "\n";	
		}
	}

	return fstring.str() + lex.get_file_content();
}
