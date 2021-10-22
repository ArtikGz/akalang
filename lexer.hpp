#pragma once
#include <string>
#include <vector>
#include "utils.hpp"
#include "token.hpp"

class Lexer {
private:
	std::string file_content;
	std::vector<Token> tokens;
	long index;

	Token get_next_token();
	bool has_more_tokens();
	void tokenize();
	bool is_number(char c);
	bool is_letter(char c);
	bool is_num(char c);
	Token form_name(char c);
	Token form_number(char c);
	Token form_string(char c);

public:
	void set_file_content(std::string file_content);
	std::string get_file_content();
	void set_tokens(std::vector<Token> tokens);
	std::vector<Token> get_tokens();
	Token expect_next_token(Token::Type token_type, std::string error_msg);
	Token next_token();
	Token explore_next_token();
	Token explore_last_token();
	bool is_parsed();
	static Lexer from_content(std::string content);
	Lexer(std::string filepath);
	Lexer();
};