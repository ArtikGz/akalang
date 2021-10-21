#include <iostream>
#include <cstring>
#include "parser.hpp"

Parser::Parser(Lexer* lexer) : tokens(lexer->get_tokens()), lexer(lexer) {}
std::vector<Statement*> Parser::parse_code() {
	std::vector<Statement*> fnc_vector;
	while (!lexer->is_parsed()) {
		lexer->next_token();
		fnc_vector.push_back(parse_function());
	}

	return fnc_vector;
}

Statement* Parser::parse_function() {
	Statement* stmt = new Statement(); 
	stmt->fnc = new Func_Def();
	stmt->type = STMT_TYPE_FUNCTION_DECLARATION;

	Token token = lexer->expect_next_token(Token::Type::TOKEN_TYPE_NAME, "Parsing error: expected name after fnc keyword");
	stmt->fnc->name = token.get_value();

	lexer->expect_next_token(Token::Type::TOKEN_TYPE_OPEN_PAREN, "Parsing error: expected open paren after function declaration");

	token = lexer->next_token();
	if (token.get_type() != Token::Type::TOKEN_TYPE_CLOSE_PAREN && token.get_type() != Token::Type::TOKEN_TYPE_NAME){
		Utils::error("Parsing error: expected name or close parent on function arguments");
	}

	if (token.get_type() == Token::Type::TOKEN_TYPE_NAME) {
		stmt->fnc->arguments = parse_fnc_arguments(token);
	} 

	lexer->expect_next_token(Token::Type::TOKEN_TYPE_GREATER_THAN, "Parsing error: expected type after function arguments");
	token = lexer->expect_next_token(Token::Type::TOKEN_TYPE_NAME, "Parsing error: untyped functions are not allowed");
	stmt->fnc->return_type = get_type_from_string(token.get_value());

	lexer->expect_next_token(Token::Type::TOKEN_TYPE_OPEN_CURLY, "Parsing error: expected block after function declaration");
	stmt->fnc->body = parse_block();

	return stmt;
}

std::vector<Func_Arg*> Parser::parse_fnc_arguments(Token name_token) {
	std::vector<Func_Arg*> function_arguments;
	while (true) {
		Func_Arg* argument = new Func_Arg();
		argument->name = name_token.get_value();
		lexer->expect_next_token(Token::Type::TOKEN_TYPE_COLON, "Parsing error: untyped function params are not allowed");
		Token token = lexer->expect_next_token(Token::Type::TOKEN_TYPE_NAME, "Parsing error: untyped function params are not allowed");
		argument->type = get_type_from_string(token.get_value());
		function_arguments.push_back(argument);

		name_token = lexer->next_token();
		if (name_token.get_type() == Token::Type::TOKEN_TYPE_CLOSE_PAREN) {
			break;
		}
		if (name_token.get_type() != Token::Type::TOKEN_TYPE_COMMA) {
			Utils::error("Parsing error: expected COMMA as argument separator on token: " + name_token.get_value());
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

std::vector<Statement*> Parser::parse_block() {
	bool unfinished_block = true;
	std::vector<Statement*> block;
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

Statement* Parser::parse_return() {
	Statement* ret = new Statement();
	ret->type = STMT_TYPE_RETURN;
	ret->expr = parse_expr(lexer->next_token());
	return ret;
}

Statement* Parser::parse_var() {
	Statement* var = new Statement();
	var->var = new Var_Asign();
	var->type = STMT_TYPE_VAR_DECLARATION;
	Token token = lexer->expect_next_token(Token::Type::TOKEN_TYPE_NAME, "Parsing error: expected name after var keyword");
	var->var->name = token.get_value();
	lexer->expect_next_token(Token::Type::TOKEN_TYPE_COLON, "Parsing error: untyped variables are not allowed");
	token = lexer->expect_next_token(Token::Type::TOKEN_TYPE_NAME, "Parsing error: untyped variables are not allowed");
	var->var->type = get_type_from_string(token.get_value());
	lexer->expect_next_token(Token::Type::TOKEN_TYPE_EQUALS, "Parsing error: expected expresion after variable declaration");
	var->var->value = parse_expr(lexer->next_token());
	return var;
}

Statement* Parser::parse_name(Token token) {
	Token ntoken = lexer->next_token();
	switch (ntoken.get_type()) {
		case Token::Type::TOKEN_TYPE_EQUALS: return parse_var_reasignation(token);
		case Token::Type::TOKEN_TYPE_OPEN_PAREN: {
			Statement* stmt = new Statement();
			stmt->expr = new Expr();
			stmt->type = STMT_TYPE_EXPR;
			stmt->expr->type = EXPR_TYPE_FUNC_CALL;
			stmt->expr->func_call = parse_func_call(token);
			return stmt;
		}
		default: Utils::error("Parsing error: couldn't parse expression");
	}
}

Statement* Parser::parse_var_reasignation(Token name) {
	Statement* stmt = new Statement();
	stmt->var = new Var_Asign();
	stmt->type = STMT_TYPE_VAR_REASIGNATION;
	stmt->var->name = name.get_value();
	stmt->var->value = parse_expr(lexer->next_token());
	return stmt;
}

Func_Call* Parser::parse_func_call(Token name) {
	Func_Call* func_call = new Func_Call();
	func_call->name = name.get_value();

	Token token = lexer->next_token();
	if (token.get_type() != Token::Type::TOKEN_TYPE_CLOSE_PAREN 
		&& token.get_type() == Token::Type::TOKEN_TYPE_NAME
		|| token.get_type() == Token::Type::TOKEN_TYPE_LITERAL_NUMBER
		|| token.get_type() == Token::Type::TOKEN_TYPE_LITERAL_STRING) {
		func_call->expr = parse_func_call_args(token);
	}

	return func_call;
}

std::vector<Expr*> Parser::parse_func_call_args(Token token) {
	std::vector<Expr*> func_call_args;
	func_call_args.push_back(parse_expr(token));
	while (true) {
		token = lexer->next_token();

		if (token.get_type() == Token::Type::TOKEN_TYPE_CLOSE_PAREN) {
			break;
		}
		if (token.get_type() != Token::Type::TOKEN_TYPE_COMMA) {
			Utils::error("Expected COMMA function as argument separator, got " + token.get_value());
		}
		token = lexer->next_token();
		func_call_args.push_back(parse_expr(token));
	}

	return func_call_args;
}

Expr* Parser::parse_expr(Token token) {
	return parse_expr_with_precedence(token, OP_PREC_0);
}

Expr* Parser::parse_primary_expr(Token token) {
	static_assert(EXPR_TYPE_COUNTER == 6, "Unhandled EXPR_TYPE_FUNC_COUNT on parse_expr() on file parser.cpp");
	Expr* expr = new Expr();
	switch (token.get_type()) {
		case Token::Type::TOKEN_TYPE_NAME:
			if (token.get_value() == "true") {
				expr->type = EXPR_TYPE_LITERAL_BOOL;
				expr->boolean = true;
				return expr;
			} else if (token.get_value() == "false") {
				expr->type = EXPR_TYPE_LITERAL_BOOL;
				expr->boolean = false;
				return expr;
			} else {
				Token ntoken = lexer->explore_next_token();
				if (ntoken.get_type() == Token::Type::TOKEN_TYPE_OPEN_PAREN) {
					lexer->next_token(); // skip TOKEN_TYPE_OPEN_PAREN
					expr->type = EXPR_TYPE_FUNC_CALL;
					expr->func_call = parse_func_call(token);
					return expr;
				} else {
					expr->type = EXPR_TYPE_VAR_READ;
					expr->var_read = token.get_value();
					return expr;
				}
			}
		case Token::Type::TOKEN_TYPE_LITERAL_NUMBER: {
			expr->type = EXPR_TYPE_LITERAL_NUMBER;
			expr->number = std::atoi(token.get_value().c_str());
			return expr;
		}

		case Token::Type::TOKEN_TYPE_LITERAL_STRING: {
			expr->type = EXPR_TYPE_LITERAL_STRING;
			expr->string = token.get_value(); // TODO add string parse
			return expr;
		}
	}

	Utils::error("Unexpected parsing expression: " + token.get_value());
}

Expr* Parser::parse_expr_with_precedence(Token token, OpPrec prec) {
	if (prec >= OP_PREC_COUNT) {
		return parse_primary_expr(token);
	}

	Expr* expr = new Expr();
	Expr* lhs = parse_expr_with_precedence(token, (OpPrec) (prec + 1));

	token = lexer->explore_next_token();
	if (is_op(token) && get_prec_by_op_type(get_op_type_by_token_type(token)) == prec) {
		lexer->next_token();
		expr->type = EXPR_TYPE_OP;
		Op* op = new Op();
		op->type = get_op_type_by_token_type(token);
		op->lhs = lhs;
		op->rhs = parse_expr_with_precedence(lexer->next_token(), prec);

		expr->op = op;
	} else {
		expr = lhs;
	}

	return expr;
}

OpType Parser::get_op_type_by_token_type(Token token) {
	static_assert(OP_TYPE_COUNT == 4, "Unhandled OP_TYPE_COUNT on get_op_type_by_token_type at parser.cpp");
	switch (token.get_type()) {
		case Token::Type::TOKEN_TYPE_ADD: return OP_TYPE_ADD;
		case Token::Type::TOKEN_TYPE_SUB: return OP_TYPE_SUB;
		case Token::Type::TOKEN_TYPE_MUL: return OP_TYPE_MUL;
		case Token::Type::TOKEN_TYPE_DIV: return OP_TYPE_DIV;
		default: Utils::error("Unknown token type: " + token.get_value());
	}
}

OpPrec Parser::get_prec_by_op_type(OpType op_type) {
	static_assert(OP_TYPE_COUNT == 4, "Unhandled OP_TYPE_COUNT on get_prec_by_op_type at parser.cpp");
	switch (op_type) {
		case OP_TYPE_ADD: return OP_PREC_0;
		case OP_TYPE_SUB: return OP_PREC_0;
		case OP_TYPE_MUL: return OP_PREC_1;
		case OP_TYPE_DIV: return OP_PREC_1;
		default: Utils::error("Unknown operation type");
	}
}

bool Parser::is_op(Token token) {
	static_assert(OP_TYPE_COUNT == 4, "Unhandled OP_TYPE_COUNT on get_op_type_by_token_type at parser.cpp");
	switch (token.get_type()) {
		case Token::Type::TOKEN_TYPE_ADD:
		case Token::Type::TOKEN_TYPE_SUB:
		case Token::Type::TOKEN_TYPE_MUL:
		case Token::Type::TOKEN_TYPE_DIV:
			return true;

		default: return false;
	}
}
