#include "parser.hpp"

Parser::Parser(Lexer* lexer) : tokens(lexer->get_tokens()), lexer(lexer) {}
std::vector<Statement> Parser::parse_code() {
	std::vector<Statement> fnc_vector;
	while (!lexer->is_parsed()) {
		lexer->next_token();
		fnc_vector.push_back(parse_function());
	}

	return fnc_vector;
}

Statement Parser::parse_function() {
	Statement stmt; 
	stmt.type = STMT_TYPE_FUNCTION_DECLARATION;

	Token token = lexer->expect_next_token(Token::Type::TOKEN_TYPE_NAME, "Parsing error: expected name after fnc keyword");
	stmt.as.fnc.name = token.get_value();

	lexer->expect_next_token(Token::Type::TOKEN_TYPE_OPEN_PAREN, "Parsing error: expected open paren after function declaration");

	token = lexer->next_token();
	if (token.get_type() != Token::Type::TOKEN_TYPE_CLOSE_PAREN && token.get_type() != Token::Type::TOKEN_TYPE_NAME){
		Utils::error("Parsing error: expected name or close parent on function arguments");
	}

	if (token.get_type() == Token::Type::TOKEN_TYPE_NAME) {
		stmt.as.fnc.arguments = parse_fnc_arguments(token);
	} 

	lexer->expect_next_token(Token::Type::TOKEN_TYPE_GREATER_THAN, "Parsing error: expected type after function arguments");
	token = lexer->expect_next_token(Token::Type::TOKEN_TYPE_NAME, "Parsing error: untyped functions are not allowed");
	stmt.as.fnc.return_type = get_type_from_string(token.get_value());

	lexer->expect_next_token(Token::Type::TOKEN_TYPE_OPEN_CURLY, "Parsing error: expected block after function declaration");
	stmt.as.fnc.body = parse_block();

	return stmt;
}

std::vector<Func_Arg> Parser::parse_fnc_arguments(Token name_token) {
	std::vector<Func_Arg> function_arguments;
	while (true) {
		Func_Arg argument;
		argument.name = name_token.get_value();
		lexer->expect_next_token(Token::Type::TOKEN_TYPE_COLON, "Parsing error: untyped function params are not allowed");
		Token token = lexer->expect_next_token(Token::Type::TOKEN_TYPE_NAME, "Parsing error: untyped function params are not allowed");
		argument.type = get_type_from_string(token.get_value());
		function_arguments.push_back(argument);

		name_token = lexer->next_token();
		if (name_token.get_type() == Token::Type::TOKEN_TYPE_CLOSE_PAREN) {
			break;
		}
		if (name_token.get_type() != Token::Type::TOKEN_TYPE_COMMA) {
			Utils::error("Parsing error: expected COMMA as argument separator");
		}
		name_token = lexer->expect_next_token(Token::Type::TOKEN_TYPE_NAME, "Parsing error: expected name after COMMA separator");
	}

	return function_arguments;
}

VarType Parser::get_type_from_string(std::string val) {
	static_assert(VAR_TYPE_COUNTER == 4, "Unhandled VAR_TYPE_COUNTER on get_type_from_string");
	if (val == "int") {
		return VAR_TYPE_INT;
	} else if (val == "bool") {
		return VAR_TYPE_BOOL;
	} else if (val == "long") {
		return VAR_TYPE_LONG;
	} else if (val == "str") {
		return VAR_TYPE_STR;
	} else {
		Utils::error("Parsing error: unknown type (types allowed: int, bool, long, str)");
	}
}

std::vector<Statement> Parser::parse_block() {
	bool unfinished_block = true;
	std::vector<Statement> block;
	Token token = lexer->next_token();
	while (unfinished_block) {
		switch (token.get_type()) {
			case Token::Type::TOKEN_TYPE_NAME: 
				block.push_back(parse_name(token)); // This can be FUNC_CALL or VAR_REASIGNATION 
				break;
			case Token::Type::TOKEN_TYPE_RETURN:
				block.push_back(parse_return());
				break;
			case Token::Type::TOKEN_TYPE_VAR:
				block.push_back(parse_var());
				break;
			case Token::Type::TOKEN_TYPE_CLOSE_CURLY: 
				unfinished_block = false;
				break;
			default: Utils::error("Parsing error: couldn't parse expression");
		}

		if (token.get_type() != Token::Type::TOKEN_TYPE_CLOSE_CURLY) {
			lexer->expect_next_token(Token::Type::TOKEN_TYPE_SEMICOLON, "Parsing error: expected semicolon at the end of the instruction");
			token = lexer->next_token();
		}
	}

	return block;
}

Statement Parser::parse_return() {
	Statement ret;
	ret.type = STMT_TYPE_RETURN;
	ret.as.expr = parse_expr(lexer->next_token());
	return ret;
}

Statement Parser::parse_var() {
	Statement var;
	var.type = STMT_TYPE_VAR_DECLARATION;
	Token token = lexer->expect_next_token(Token::Type::TOKEN_TYPE_NAME, "Parsing error: expected name after var keyword");
	var.as.var.name = token.get_value();
	lexer->expect_next_token(Token::Type::TOKEN_TYPE_COLON, "Parsing error: untyped variables are not allowed");
	token = lexer->expect_next_token(Token::Type::TOKEN_TYPE_NAME, "Parsing error: untyped variables are not allowed");
	var.as.var.type = get_type_from_string(token.get_value());
	lexer->expect_next_token(Token::Type::TOKEN_TYPE_EQUALS, "Parsing error: expected expresion after variable declaration");
	var.as.var.value = parse_expr(lexer->next_token());
	return var;
}

Statement Parser::parse_name(Token token) {
	Token ntoken = lexer->next_token();
	switch (ntoken.get_type()) {
		case Token::Type::TOKEN_TYPE_EQUALS: return parse_var_reasignation(token);
		case Token::Type::TOKEN_TYPE_OPEN_PAREN: {
			Statement stmt;
			stmt.type = STMT_TYPE_EXPR;
			stmt.as.expr.type = EXPR_TYPE_FUNC_CALL;
			stmt.as.expr.as.func_call = parse_func_call(token);
			return stmt;
		}
		default: Utils::error("Parsing error: couldn't parse expression");
	}
}

Statement Parser::parse_var_reasignation(Token name) {
	Statement stmt;
	stmt.type = STMT_TYPE_VAR_REASIGNATION;
	stmt.as.var.name = name.get_value();
	stmt.as.var.value = parse_expr(lexer->next_token());
	return stmt;
}

Func_Call Parser::parse_func_call(Token name) {
	Func_Call func_call;
	func_call.name = name.get_value();

	Token token = lexer->next_token();
	if (token.get_type() != Token::Type::TOKEN_TYPE_CLOSE_PAREN 
		&& token.get_type() == Token::Type::TOKEN_TYPE_NAME
		|| token.get_type() == Token::Type::TOKEN_TYPE_LITERAL_NUMBER
		|| token.get_type() == Token::Type::TOKEN_TYPE_LITERAL_STRING) {
		func_call.expr = parse_func_call_args(token);
	}

	return func_call;
}

std::vector<Expr> Parser::parse_func_call_args(Token token) {
	std::vector<Expr> func_call_args;
	func_call_args.push_back(parse_expr(token));
	while (true) {
		token = lexer->next_token();

		if (token.get_type() == Token::Type::TOKEN_TYPE_CLOSE_PAREN) {
			break;
		}
		if (token.get_type() != Token::Type::TOKEN_TYPE_COMMA) {
			Utils::error("Expected COMMA function as argument separator");
		}
		token = lexer->next_token();
		func_call_args.push_back(parse_expr(token));
	}

	return func_call_args;
}

Expr Parser::parse_expr(Token token) {
	static_assert(EXPR_TYPE_COUNTER == 5, "Unhandled EXPR_TYPE_FUNC_COUNT on parse_expr() on file parser.cpp");
	switch (token.get_type()) {
		case Token::Type::TOKEN_TYPE_NAME:
			if (token.get_value() == "true") {
				Expr expr;
				expr.type = EXPR_TYPE_LITERAL_BOOL;
				expr.as.boolean = true;
				return expr;
			} else if (token.get_value() == "false") {
				Expr expr;
				expr.type = EXPR_TYPE_LITERAL_BOOL;
				expr.as.boolean = false;
				return expr;
			} else {
				Token ntoken = lexer->explore_next_token();
				if (ntoken.get_type() == Token::Type::TOKEN_TYPE_OPEN_PAREN) {
					lexer->next_token(); // skip TOKEN_TYPE_OPEN_PAREN
					Expr expr;
					expr.type = EXPR_TYPE_FUNC_CALL;
					expr.as.func_call = parse_func_call(token);
					return expr;
				} else {
					Expr expr;
					expr.type = EXPR_TYPE_VAR_READ;
					expr.as.var_read = token.get_value();
					return expr;
				}
			}
		case Token::Type::TOKEN_TYPE_LITERAL_NUMBER: {
			Expr expr;
			expr.type = EXPR_TYPE_LITERAL_NUMBER;
			expr.as.number = std::atoi(token.get_value().c_str());
			return expr;
		}

		case Token::Type::TOKEN_TYPE_LITERAL_STRING: {
			Expr expr;
			expr.type = EXPR_TYPE_LITERAL_STRING;
			expr.as.string = token.get_value(); // TODO add string parse
			return expr;
		}
	}

	Utils::error("Unexpected parsing expression");
}