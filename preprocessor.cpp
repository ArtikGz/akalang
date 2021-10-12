#include "preprocessor.hpp"
#include "utils.hpp"
#include "lexer.hpp"
#include "token.hpp"

std::string Preprocessor::preprocess_includes(std::string filename) {
	std::string preprocessed_includes;

	std::vector<std::string> filenames;
	std::string content = Utils::read_file(filename);
	Lexer lexer = Lexer(filename);
	Token token = lexer.next_token();
	while (token.get_type() == Token::Type::TOKEN_TYPE_INCLUDE_DIRECTIVE) {
		token = lexer.expect_next_token(Token::Type::TOKEN_TYPE_LITERAL_STRING, "Preprocessor error: expected filename after include directive");
		// Avoid importing two times the same file
		for (const std::string& file: filenames) {
			if (file == token.get_value()) {
				Utils::error("Preprocessor error: importing same file more than one time is not allowed, " + token.get_value());
			}
		}

		lexer.expect_next_token(Token::Type::TOKEN_TYPE_SEMICOLON, "Preprocessor error: expected semicolon at the end of include directive");
		filenames.push_back(token.get_value());

		// Remove include directive from file
		lexer.set_file_content(lexer.get_file_content().erase(0, 11 + token.get_value().length()));

		token = lexer.next_token();
	}

	for (const std::string& file: filenames) {
		preprocessed_includes += preprocess_includes(file);
	}
	preprocessed_includes += lexer.get_file_content();
	return preprocessed_includes;
}