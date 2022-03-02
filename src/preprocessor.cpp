#include <iostream>
#include <vector>
#include "preprocessor.hpp"
#include "utils.hpp"
#include "lexer.hpp"
#include "token.hpp"

std::string Preprocessor::preprocess_includes(const std::string& filename, std::vector<std::string>& global_filenames) {
	std::string preprocessed_includes;
	std::vector<std::string> filenames;

	Lexer lexer = Lexer(filename);
	Token token = lexer.next_token();
	while (token.get_type() == Token::Type::INCLUDE_DIRECTIVE) {
		token = lexer.expect_next_token(Token::Type::LITERAL_STRING, "Preprocessor error: expected filename after include directive");
		// Avoid importing two times the same file
		bool repeated = false;
		for (const std::string& file: global_filenames) {
			if (file == token.get_value()) {
				repeated = true;
				break;
			}
		}

		lexer.expect_next_token(Token::Type::SEMICOLON, "Preprocessor error: expected semicolon at the end of include directive");
		if (!repeated) {
			filenames.push_back(token.get_value());
			global_filenames.push_back(token.get_value());
		}
		// Remove include directive from file
		lexer.set_file_content(lexer.get_file_content().erase(0, lexer.get_file_content().find_first_of(";") + 1));

		token = lexer.next_token();
	}

	for (const std::string& file: filenames) {
		preprocessed_includes += preprocess_includes(file, filenames);
		preprocessed_includes += "\n";
	}
	preprocessed_includes += lexer.get_file_content();
	return preprocessed_includes;
}
