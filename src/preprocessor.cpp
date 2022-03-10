#include <iostream>
#include <vector>
#include <sstream>
#include "preprocessor.hpp"
#include "utils.hpp"
#include "lexer.hpp"
#include "token.hpp"

void Preprocessor::preprocess_includes(const std::string& filename, std::vector<std::string>& global_filenames, std::vector<Token>& tokens) {
	std::stringstream fstring;

	Lexer lex(filename);
	std::vector<Token> toks = lex.get_tokens();
	while (lex.next_token().get_type() == Token::Type::INCLUDE_DIRECTIVE) {
		std::string name = lex.expect_next_token(Token::Type::LITERAL_STRING, "Preprocessing error: expected string after include").get_value();
		lex.expect_next_token(Token::Type::SEMICOLON, "Preprocessing error: expected semicolon after include directive");

		toks.erase(toks.begin());
		toks.erase(toks.begin());
		toks.erase(toks.begin());

		bool skip = false;
		for (auto n: global_filenames) {
			if (n == name) {
				skip = true;
				break;
			}
		}

		if (!skip) {
			global_filenames.push_back(name);
			preprocess_includes(name, global_filenames, tokens);
		}
	}

	tokens.reserve(tokens.size() + toks.size());
	tokens.insert(tokens.end(), toks.begin(), toks.end());
}
