#include <iostream>
#include "lexer.hpp"

Token Lexer::get_next_token() {
	Token token;
	long start = index;
	token.set_type(trie.lookup(file_content, index));
	if (token.get_type() != Token::Type::UNKNOWN) {
		token.set_value(file_content.substr(start, index - start));
	}

	if (token.get_type() == Token::Type::UNKNOWN) {
		if (is_letter(file_content[index])) {
			token.set_type(Token::Type::NAME);
			token.set_value(form_name());
		} else if (is_number(file_content[index])) {
			token.set_type(Token::Type::LITERAL_NUMBER);
			token.set_value(form_number());
		} else if (file_content[index] == '"') {
			token.set_type(Token::Type::LITERAL_STRING);
			token.set_value(form_string());
		} else {
			Utils::error("Unknown token found: \"" + file_content.substr(index, file_content.size() - index) + "\"");
		}
	}
	return token;
}

bool Lexer::has_more_tokens() {
	return this->index < file_content.size();
}

void Lexer::tokenize() {
	while (has_more_tokens()) {
		skip_whitespace();
		if (!has_more_tokens()) {
			break;
		}

		tokens.push_back(get_next_token());
	}

	this->index = 0;
}

bool Lexer::is_number(char c) {
	return c >= '0' && c <= '9';
}

bool Lexer::is_letter(char c) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
}

void Lexer::skip_whitespace() {
	while (Utils::is_blankspace(file_content[index]) && index < file_content.size()) {
		index += 1;
	}
}

std::string Lexer::form_name() {
	long start = index;
	while (is_letter(file_content[index]) || is_number(file_content[index])) {
		index += 1;
	}

	return file_content.substr(start, index - start);
}

std::string Lexer::form_number() {
	long start = index;
	while (is_number(file_content[index])) {
		index += 1;
	}

	return file_content.substr(start, index - start);
}

std::string Lexer::form_string() {
	long start = index++;
	while (file_content[index] != '"') {
		index++;
	}
	index++;

	return file_content.substr(start + 1, index - start - 2);
}

Token Lexer::expect_next_token(Token::Type token_type, std::string error_msg) {
	if (this->tokens[index].get_type() == token_type) {
		return this->tokens[index++];
	} else {
		Utils::error(error_msg);
		exit(1);
	}
}

Token Lexer::next_token() {
	return this->tokens[index++];
}

Token Lexer::explore_next_token() {
	return this->tokens[index];
}

Token Lexer::explore_last_token() {
	return this->tokens[index - 1];
}

bool Lexer::is_parsed() {
	return this->tokens.size() <= index;
}

void Lexer::register_keywords() {
	trie.add_keyword("(", Token::Type::OPEN_PAREN);
	trie.add_keyword(")", Token::Type::CLOSE_PAREN);
	trie.add_keyword("[", Token::Type::OPEN_BRACKET);
	trie.add_keyword("]", Token::Type::CLOSE_BRACKET);
	trie.add_keyword("{", Token::Type::OPEN_CURLY);
	trie.add_keyword("}", Token::Type::CLOSE_CURLY);
	trie.add_keyword(">", Token::Type::GREATER_THAN);
	trie.add_keyword("<", Token::Type::LOWER_THAN);
	trie.add_keyword(";", Token::Type::SEMICOLON);
	trie.add_keyword(":", Token::Type::COLON);
	trie.add_keyword(",", Token::Type::COMMA);
	trie.add_keyword("=", Token::Type::EQUALS);
	trie.add_keyword("==", Token::Type::EQUALS_COMPARE);
	trie.add_keyword("<=", Token::Type::LOWER_THAN_EQUALS);
	trie.add_keyword("!=", Token::Type::BANG_EQUALS);
	trie.add_keyword("-", Token::Type::SUB);
	trie.add_keyword("+", Token::Type::ADD);
	trie.add_keyword("/", Token::Type::DIV);
	trie.add_keyword("*", Token::Type::MUL);
	trie.add_keyword("%", Token::Type::MOD);
	trie.add_keyword("function", Token::Type::FUNCTION);
	trie.add_keyword("return", Token::Type::RETURN);
	trie.add_keyword("var", Token::Type::VAR);
	trie.add_keyword("if", Token::Type::IF);
	trie.add_keyword("for", Token::Type::FOR);
	trie.add_keyword("include", Token::Type::INCLUDE_DIRECTIVE);
	trie.add_keyword("else", Token::Type::ELSE);
	trie.add_keyword("while", Token::Type::WHILE);
	trie.add_keyword("->", Token::Type::ARROW);
}

void Lexer::return_index() {
	index -= 1;
}

void Lexer::set_file_content(std::string file_content) { this->file_content = file_content; }
std::string Lexer::get_file_content() { return this->file_content; }
void Lexer::set_tokens(std::vector<Token> tokens) { this->tokens = tokens; }
std::vector<Token> Lexer::get_tokens() { return this->tokens; }

Lexer::Lexer() {}
Lexer::Lexer(std::string filepath) : file_content(Utils::read_file(filepath)) {
	this->index = 0;
	register_keywords();
	tokenize();
}

std::unique_ptr<Lexer> Lexer::from_content(std::string content) {
  std::unique_ptr<Lexer> lexer = std::make_unique<Lexer>();
	lexer->set_file_content(content);
	lexer->index = 0;
	lexer->register_keywords();
	lexer->tokenize();
	return lexer;
}

static_assert(Token::Type::TOKEN_COUNTER == 33, "Unhandled TOKEN_COUNTER on lexer.cpp");
