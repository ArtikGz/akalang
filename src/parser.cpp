#include <iostream>
#include <cstring>
#include "parser.hpp"

Parser::Parser(std::unique_ptr<Lexer>&& lexer) : tokens(lexer->get_tokens()), lexer(std::move(lexer)) {}
std::vector<std::shared_ptr<Statement>> Parser::parse_code() {
	std::vector<std::shared_ptr<Statement>> stmt_vector;
	while (!lexer->is_parsed()) {
		Token token = lexer->next_token();
		switch (token.get_type()) {
			case Token::Type::FUNCTION:
				stmt_vector.push_back(parse_function());
				break;

			default:
				Utils::error("Unknown top level expression");
				exit(1);
		}
	}

	return stmt_vector;
}

std::shared_ptr<Statement> Parser::parse_function() {
	std::shared_ptr<Statement> stmt = std::make_shared<Statement>(); 
	stmt->fnc = std::make_shared<Func_Def>();
	stmt->type = STMT_TYPE_FUNCTION_DECLARATION;

	Token token = lexer->expect_next_token(Token::Type::NAME, "Parsing error: expected name after function keyword, but got " + lexer->explore_last_token().get_value());
	stmt->fnc->name = token.get_value();

	lexer->expect_next_token(Token::Type::OPEN_PAREN, "Parsing error: expected open paren after function declaration");

	token = lexer->next_token();
	if (token.get_type() != Token::Type::CLOSE_PAREN && token.get_type() != Token::Type::NAME){
		Utils::error("Parsing error: expected name or close parent on function arguments");
	}

	if (token.get_type() == Token::Type::NAME) {
		stmt->fnc->arguments = parse_fnc_arguments(token);
	} 

	lexer->expect_next_token(Token::Type::ARROW, "Parsing error: expected type after function arguments");

	size_t stars = count_stars();
	std::string typestr = lexer->expect_next_token(Token::Type::NAME, "Parsing error: expected a name as function type").get_value();
	stmt->fnc->return_type = get_type_from_string(stars, typestr);

	lexer->expect_next_token(Token::Type::OPEN_CURLY, "Parsing error: expected block after function declaration");
	stmt->fnc->body = parse_block();

	return stmt;
}

std::vector<std::shared_ptr<Func_Arg>> Parser::parse_fnc_arguments(Token name_token) {
	std::vector<std::shared_ptr<Func_Arg>> function_arguments;
	while (true) {
		std::shared_ptr<Func_Arg> argument = std::make_shared<Func_Arg>();
		argument->name = name_token.get_value();
		lexer->expect_next_token(Token::Type::COLON, "Parsing error: expected colon after name on function definition arguments");

		size_t stars = count_stars();
		std::string datastr = lexer->expect_next_token(Token::Type::NAME, "Parsing error: expected name after colon on function definition arguments").get_value();
		argument->type = get_type_from_string(stars, datastr);

		function_arguments.push_back(argument);

		name_token = lexer->next_token();
		if (name_token.get_type() == Token::Type::CLOSE_PAREN) {
			break;
		}
		if (name_token.get_type() != Token::Type::COMMA) {
			Utils::error("Parsing error: expected COMMA as argument separator on token: " + name_token.get_value());
		}
		name_token = lexer->expect_next_token(Token::Type::NAME, "Parsing error: expected name after COMMA separator");
	}

	return function_arguments;
}

size_t Parser::count_stars() {
  int counter = 0;
  while (lexer->explore_next_token().get_type() == Token::Type::MUL) {
    lexer->next_token();
    counter += 1;
  }

  return counter;
}

VarType Parser::get_type_from_string(size_t stars, std::string val) {
	static_assert(VAR_TYPE_COUNTER == 5, "Unhandled VAR_TYPE_COUNTER on get_type_from_string");
	VarType varType;
	varType.stars = stars;

	if (val == "int") {
		varType.type = VAR_TYPE_INT;
	} else if (val == "bool") {
		varType.type = VAR_TYPE_BOOL;
	} else if (val == "long") {
		varType.type = VAR_TYPE_LONG;
	} else if (val == "char") {
		varType.type = VAR_TYPE_CHAR;
	} else {
		Utils::error("Parsing error: unknown type (types allowed: int, bool, long, char), but got " + val);
	}

  return varType;
}

std::vector<std::shared_ptr<Statement>> Parser::parse_block() {
	static_assert(STMT_TYPE_COUNTER == 7, "Unhandled STMT_TYPE_COUNTER on parse_block() at parser.cpp");
	bool unfinished_block = true;
	std::vector<std::shared_ptr<Statement>> block;

	while (unfinished_block) {
		Token token = lexer->next_token();
		switch (token.get_type()) {
			case Token::Type::NAME: 
				block.push_back(parse_name()); // This can be FUNC_CALL or VAR_REASIGNATION 
				break;
			case Token::Type::RETURN:
				block.push_back(parse_return());
				break;
			case Token::Type::VAR:
				block.push_back(parse_var());
				break;
			case Token::Type::IF:
				block.push_back(parse_if());
				continue; 
			case Token::Type::WHILE:
				block.push_back(parse_while());
				continue;
			case Token::Type::MUL: {
				// if extra stars report an error and exit
				size_t stars = count_stars();
				if (stars != 0) {
					Utils::error("Parsing error: unexpected number of stars on left hand side");
					exit(1);
				}

				std::shared_ptr<Var_Asign> var = std::make_shared<Var_Asign>();
				var->name = lexer->expect_next_token(Token::Type::NAME, "Parsing error: expected name after star on statement").get_value();
				var->is_ptr = true;
				lexer->expect_next_token(Token::Type::EQUALS, "Parsing error: expected equals after left hand side on pointer var reasignation");
				var->value = parse_expr(lexer->next_token());

				std::shared_ptr<Statement> stmt = std::make_shared<Statement>();
				stmt->type = STMT_TYPE_VAR_REASIGNATION;
				stmt->var = var;
				block.push_back(stmt);
				break;
			}
			case Token::Type::CLOSE_CURLY: 
				unfinished_block = false;
				break;
			case Token::Type::SEMICOLON:
				// Allows optional semicolon at end of the statement
				break;
			default:
				Utils::error("Parsing error: couldn't parse expression " + token.get_value());
		}
	}

	return block;
}

std::shared_ptr<Statement> Parser::parse_while() {
	std::shared_ptr<Statement> stmt = std::make_shared<Statement>();
	stmt->type = STMT_TYPE_WHILE;
	stmt->whilee = std::make_shared<While>();
	stmt->whilee->condition = parse_expr(lexer->next_token());
	lexer->expect_next_token(Token::Type::OPEN_CURLY, "Parsing error: expected open curly after if condition, but got " + lexer->explore_last_token().get_value());
	stmt->whilee->block = parse_block();
	return stmt;
}

std::shared_ptr<Statement> Parser::parse_if() {
	std::shared_ptr<Statement> stmt = std::make_shared<Statement>();
	stmt->type = STMT_TYPE_IF;
	stmt->iif = std::make_shared<If>();
	stmt->iif->condition = parse_expr(lexer->next_token());
	lexer->expect_next_token(Token::Type::OPEN_CURLY, "Parsing error: expected open curly after if condition, but got " + lexer->explore_last_token().get_value());
	stmt->iif->then = parse_block();
	Token token = lexer->explore_next_token();
	if (token.get_type() == Token::Type::ELSE) {
		lexer->next_token();
		lexer->expect_next_token(Token::Type::OPEN_CURLY, "Parsing error: expected open curly after else statement");
		stmt->iif->elsse = parse_block();
	}

	return stmt;
}

std::shared_ptr<Statement> Parser::parse_return() {
	std::shared_ptr<Statement> ret = std::make_shared<Statement>();
	ret->type = STMT_TYPE_RETURN;
	ret->expr = parse_expr(lexer->next_token());
	return ret;
}

std::shared_ptr<Statement> Parser::parse_var() {
	std::shared_ptr<Statement> var = std::make_shared<Statement>();
	var->var = std::make_shared<Var_Asign>();
	var->type = STMT_TYPE_VAR_DECLARATION;
	Token token = lexer->expect_next_token(Token::Type::NAME, "Parsing error: expected name after var keyword");
	var->var->name = token.get_value();
	lexer->expect_next_token(Token::Type::COLON, "Parsing error: missing semicolon after var name");

	size_t stars = count_stars();
	std::string typestr = lexer->expect_next_token(Token::Type::NAME, "Parsing error: untyped variables are not allowed").get_value();
	var->var->type = get_type_from_string(stars, typestr);

	lexer->expect_next_token(Token::Type::EQUALS, "Parsing error: expected expresion after variable declaration");
	var->var->value = parse_expr(lexer->next_token());
	return var;
}

std::shared_ptr<Statement> Parser::parse_name() {
	Token token = lexer->explore_last_token();
	Token ntoken = lexer->next_token();
	switch (ntoken.get_type()) {
		case Token::Type::EQUALS: return parse_var_reasignation(token);
		case Token::Type::OPEN_PAREN: {
			std::shared_ptr<Statement> stmt = std::make_shared<Statement>();
			stmt->expr = std::make_shared<Expr>();
			stmt->type = STMT_TYPE_EXPR;
			stmt->expr->type = EXPR_TYPE_FUNC_CALL;
			stmt->expr->func_call = parse_func_call(token);
			return stmt;
		}
		default: Utils::error("Parsing error: exppected '=' or '(' symbol after using a name as an statement"); exit(1);
	}
}

std::shared_ptr<Statement> Parser::parse_var_reasignation(Token name) {
	std::shared_ptr<Statement> stmt = std::make_shared<Statement>();
	stmt->var = std::make_shared<Var_Asign>();
	stmt->type = STMT_TYPE_VAR_REASIGNATION;
	stmt->var->name = name.get_value();
	stmt->var->value = parse_expr(lexer->next_token());
	return stmt;
}

std::shared_ptr<Func_Call> Parser::parse_func_call(Token name) {
	std::shared_ptr<Func_Call> func_call = std::make_shared<Func_Call>();
	func_call->name = name.get_value();

	Token token = lexer->next_token();
	if (token.get_type() != Token::Type::CLOSE_PAREN) {
		func_call->expr = parse_func_call_args(token);
	}

	return func_call;
}

std::vector<std::shared_ptr<Expr>> Parser::parse_func_call_args(Token token) {
	std::vector<std::shared_ptr<Expr>> func_call_args;
	func_call_args.push_back(parse_expr(token));
	while (true) {
		token = lexer->next_token();

		if (token.get_type() == Token::Type::CLOSE_PAREN) {
			break;
		}
		if (token.get_type() != Token::Type::COMMA) {
			Utils::error("Expected COMMA function as argument separator, got " + token.get_value());
		}
		token = lexer->next_token();
		func_call_args.push_back(parse_expr(token));
	}

	return func_call_args;
}

std::shared_ptr<Expr> Parser::parse_expr(Token token) {
	return parse_expr_with_precedence(token, OP_PREC_0);
}

std::shared_ptr<Expr> Parser::parse_primary_expr(Token token) {
	static_assert(EXPR_TYPE_COUNTER == 6, "Unhandled EXPR_TYPE_FUNC_COUNT on parse_expr() on file parser.cpp");
	std::shared_ptr<Expr> expr = std::make_shared<Expr>();
	switch (token.get_type()) {
		case Token::Type::NAME:
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
				if (ntoken.get_type() == Token::Type::OPEN_PAREN) {
					lexer->next_token(); // skip OPEN_PAREN
					expr->type = EXPR_TYPE_FUNC_CALL;
					expr->func_call = parse_func_call(token);
					return expr;
				} else {
					expr->type = EXPR_TYPE_VAR_READ;

					Var_Read varRead;
					varRead.stars = 0;
					varRead.var_name = token.get_value();
					expr->var_read = varRead;
					return expr;
				}
			}
		case Token::Type::LITERAL_NUMBER: {
			expr->type = EXPR_TYPE_LITERAL_NUMBER;
			expr->number = std::atoi(token.get_value().c_str());
			return expr;
		}

		case Token::Type::LITERAL_STRING: {
			expr->type = EXPR_TYPE_LITERAL_STRING;
			std::string str = token.get_value();
			int strsize = str.size();
			for (int i = 0; i < strsize; i++) {
				if (str[i] == '\\') {
					switch (str[i + 1]) {
						case 'n':
							str.replace(i, i + 2, "\n");
							i += 1;
							break;
						default: Utils::error("Parsing error: unrecognized escaping sequence"); exit(1);
					}
				}
			}

			expr->string = str; 
			return expr;
		}

		case Token::Type::MUL: {
			expr->type = EXPR_TYPE_VAR_READ;
			Var_Read varRead;

			lexer->return_index();
			varRead.stars = count_stars();
			varRead.var_name = lexer->expect_next_token(Token::Type::NAME, "Parsing error: expected name after '*' symbol on expression").get_value();
			expr->var_read = varRead;
			return expr;
		}

		case Token::Type::SUB: {
			Token n = lexer->expect_next_token(Token::Type::LITERAL_NUMBER, "Parsing error: only numbers are expected after minus simbol");
			expr->type = EXPR_TYPE_LITERAL_NUMBER;
			expr->number = atoi(token.get_value().c_str());
			return expr;
		}

		default: Utils::error("Unexpected parsing expression: " + token.get_value()); exit(1);
	}
}

std::shared_ptr<Expr> Parser::parse_expr_with_precedence(Token token, OpPrec prec) {
	if (prec >= OP_PREC_COUNT) {
		return parse_primary_expr(token);
	}

	std::shared_ptr<Expr> expr = std::make_shared<Expr>();
	std::shared_ptr<Expr> lhs = parse_expr_with_precedence(token, (OpPrec) (prec + 1));

	token = lexer->explore_next_token();
	if (is_op(token) && get_prec_by_op_type(get_op_type_by_token_type(token)) == prec) {
		lexer->next_token();
		expr->type = EXPR_TYPE_OP;
		std::shared_ptr<Op> op = std::make_shared<Op>();
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
	static_assert(OP_TYPE_COUNT == 10, "Unhandled OP_TYPE_COUNT on get_op_type_by_token_type at parser.cpp");
	switch (token.get_type()) {
		case Token::Type::ADD: return OP_TYPE_ADD;
		case Token::Type::SUB: return OP_TYPE_SUB;
		case Token::Type::MUL: return OP_TYPE_MUL;
		case Token::Type::DIV: return OP_TYPE_DIV;
		case Token::Type::MOD: return OP_TYPE_MOD;
		case Token::Type::LOWER_THAN: return OP_TYPE_LT;
		case Token::Type::GREATER_THAN: return OP_TYPE_GT;
		case Token::Type::EQUALS_COMPARE: return OP_TYPE_EQ;
		case Token::Type::BANG_EQUALS: return OP_TYPE_NEQ;
		case Token::Type::LOWER_THAN_EQUALS: return OP_TYPE_LTE;
		default: Utils::error("Unknown token type: " + token.get_value()); exit(1);
	}
}

OpPrec Parser::get_prec_by_op_type(OpType op_type) {
	static_assert(OP_TYPE_COUNT == 10, "Unhandled OP_TYPE_COUNT on get_prec_by_op_type at parser.cpp");
	switch (op_type) {
		case OP_TYPE_LT:
		case OP_TYPE_GT:
		case OP_TYPE_EQ:
		case OP_TYPE_NEQ:
		case OP_TYPE_LTE:
			return OP_PREC_0;
		case OP_TYPE_ADD: 
		case OP_TYPE_SUB:
			return OP_PREC_1;
		case OP_TYPE_MUL: 
		case OP_TYPE_DIV: 
		case OP_TYPE_MOD:
			return OP_PREC_2;
		default: Utils::error("Unknown operation type"); exit(1);
	}
}

bool Parser::is_op(Token token) {
	static_assert(OP_TYPE_COUNT == 10, "Unhandled OP_TYPE_COUNT on get_op_type_by_token_type at parser.cpp");
	switch (token.get_type()) {
		case Token::Type::LOWER_THAN:
		case Token::Type::GREATER_THAN:
		case Token::Type::EQUALS_COMPARE:
		case Token::Type::BANG_EQUALS:
		case Token::Type::LOWER_THAN_EQUALS:
		case Token::Type::ADD:
		case Token::Type::SUB:
		case Token::Type::MUL:
		case Token::Type::DIV:
		case Token::Type::MOD:
			return true;

		default: return false;
	}
}
