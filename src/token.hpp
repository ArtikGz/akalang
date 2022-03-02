#pragma once
#include <string>

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

public:
	void set_type(Type type);
	Type get_type();
	void set_value(std::string value);
	std::string get_value();
};
