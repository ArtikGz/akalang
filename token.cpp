#include "token.hpp"
#include <iostream>

Token::Token(Type type, char c) : type(type), value(std::string(1, c)) {
	std::cout << "New token: " << value << std::endl;
}
Token::Token(Type type, std::string c) : type(type), value(c) {
	std::cout << "New token: " << value << std::endl;
}

void Token::set_type(Type type) { this->type = type; }
Token::Type Token::get_type() { return type; }
void Token::set_value(std::string value) { this->value = value; }
std::string Token::get_value() { return value; }