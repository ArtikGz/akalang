#pragma once
#include <string>

class TokenLoc {
public:
	size_t row;
	size_t column;
	std::string filename;

	TokenLoc(size_t row, size_t column, std::string filename) 
		: row(row), column(column), filename(filename) {}

	TokenLoc() {}
};	

class Token {
public:
	enum Type {
		UNKNOWN,
		OPEN_PAREN,
		CLOSE_PAREN,
		OPEN_BRACKET,
		CLOSE_BRACKET,
		OPEN_CURLY,
		CLOSE_CURLY,
		GREATER_THAN,
		LOWER_THAN,
		EQUALS_COMPARE,
		SEMICOLON,
		COLON,
		COMMA,
		EQUALS,
		FUNCTION,
		RETURN,
		VAR,
		FOR,
		IF,
		ELSE,
		WHILE,
		NAME,
		LITERAL_NUMBER,
		LITERAL_STRING,
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		INCLUDE_DIRECTIVE,
		ARROW,
		BANG_EQUALS,
		LOWER_THAN_EQUALS,
		TOKEN_COUNTER,
	};
	Token();

private:
	Type type;
	std::string value;
	TokenLoc loc;

public:
	void set_type(Type type);
	Type get_type();
	void set_value(std::string value);
	std::string get_value();
	void set_loc(TokenLoc loc);
	TokenLoc get_loc();
};
