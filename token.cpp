#include "token.hpp"

Token::Token() {}

void Token::set_type(Type type) { this->type = type; }
Token::Type Token::get_type() { return type; }
void Token::set_value(std::string value) { this->value = value; }
std::string Token::get_value() { return value; }