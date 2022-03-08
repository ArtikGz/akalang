#pragma once
#include <string>
#include <vector>
#include "utils.hpp"
#include "token.hpp"
#include "trie.hpp"

class Lexer {
private:
	Trie trie;
	std::string file_content;
	std::vector<Token> tokens;
	long index;

	Token get_next_token();
	bool has_more_tokens();
	void tokenize();
	bool is_number(char c);
	bool is_letter(char c);
	std::string form_name();
	std::string form_number();
	std::string form_string();
	void register_keywords();
	void skip_whitespace();

public:
	void set_file_content(std::string file_content);
	std::string get_file_content();
	void set_tokens(std::vector<Token> tokens);
	std::vector<Token> get_tokens();
	Token expect_next_token(Token::Type token_type, std::string error_msg);

	/**
	 * @brief Get next token adding one to the token list index
	 * 
	 * @return Token 
	 */
	Token next_token();

	/**
	 * @brief Substracts one to the token list index
	 * 
	 */
	void return_index();

	/**
	 * @brief Get next token without moving the token list index
	 * 
	 * @return Token 
	 */
	Token explore_next_token();

	/**
	 * @brief Get previous token without moving the token list index
	 * 
	 * @return Token 
	 */
	Token explore_last_token();
	bool is_parsed();
	static std::unique_ptr<Lexer> from_content(std::string content);
	Lexer(std::string filepath);
	Lexer();
};