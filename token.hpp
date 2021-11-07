#pragma once
#include <string>

class Token {
public:
	enum Type {
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
		TOKEN_COUNTER,
	};
	Token(Type type, char c);
	Token(Type type, std::string c);

private:
	Type type;
	std::string value;

public:
	void set_type(Type type);
	Type get_type();
	void set_value(std::string value);
	std::string get_value();
};
