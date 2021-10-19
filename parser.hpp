#pragma once
#include <string>
#include <vector>
#include <map>
#include "token.hpp"
#include "lexer.hpp"

typedef struct Func_Arg Func_Arg;
typedef struct ExprAs ExprAs;
typedef struct Expr Expr;
typedef struct Func_Def Func_Def;
typedef struct Var_Asign Var_Asign;
typedef struct Ret Ret;
typedef struct StmtAs StmtAs;
typedef struct Statement Statement;
typedef struct Op Op;

typedef enum {
	VAR_TYPE_INT,
	VAR_TYPE_LONG,
	VAR_TYPE_STR,
	VAR_TYPE_BOOL,
	VAR_TYPE_COUNTER
} VarType;

typedef enum {
	STMT_TYPE_FUNCTION_DECLARATION,
	STMT_TYPE_VAR_REASIGNATION,
	STMT_TYPE_RETURN,
	STMT_TYPE_EXPR,
	STMT_TYPE_VAR_DECLARATION,
	STMT_TYPE_COUNTER
} StmtType;

typedef enum {
	OP_TYPE_ADD,
	OP_TYPE_SUB,
	OP_TYPE_DIV,
	OP_TYPE_MUL,
	OP_TYPE_COUNT
} OpType;

typedef enum {
	OP_PREC_0,
	OP_PREC_1,
	OP_PREC_COUNT
} OpPrec;

struct Op {
	OpType type;
	Expr* lhs;
	Expr* rhs;
};


struct Func_Arg {
	VarType type;
	std::string name;
};

struct Func_Def {
	std::string name;
	VarType return_type;
	std::vector<Func_Arg> arguments;
	std::vector<Statement> body;
};

struct Func_Call {
	std::string name;
	std::vector<Expr> expr;
};

typedef enum {
	EXPR_TYPE_FUNC_CALL,
	EXPR_TYPE_LITERAL_BOOL,
	EXPR_TYPE_VAR_READ,
	EXPR_TYPE_LITERAL_NUMBER,
	EXPR_TYPE_LITERAL_STRING,
	EXPR_TYPE_OP,
	EXPR_TYPE_COUNTER
} ExprType;

struct ExprAs {
	Func_Call func_call;
	bool boolean;
	int number;
	std::string var_read;
	std::string string;
	Op op;
};
static_assert(EXPR_TYPE_COUNTER == 6, "Unhandled EXPR_TYPE_COUNTER on parser.hpp");

struct Expr {
	ExprType type;
	ExprAs as;
};

struct Var_Asign {
	std::string name;
	VarType type;
	Expr value;
};

struct Ret {
	Expr expr;
};

struct StmtAs {
	Func_Def fnc;
	Var_Asign var;
	Expr expr;
};
static_assert(STMT_TYPE_COUNTER == 5, "Unhandled STMT_TYPE_COUNTER on parser.hpp");

struct Statement {
	StmtType type;
	StmtAs as;
};

class Parser {
private:
	std::vector<Token> tokens;
	Lexer* lexer;

public:
	Parser(Lexer* lexer);
	Statement parse_function();
	std::vector<Statement> parse_block();
	std::vector<Func_Arg> parse_fnc_arguments(Token name_token);
	VarType get_type_from_string(std::string val);
	std::vector<Statement> parse_code();
	Statement parse_name(Token token);
	Statement parse_return();
	Statement parse_var_reasignation(Token name);
	Statement parse_var();
	Func_Call parse_func_call(Token name);
	std::vector<Expr> parse_func_call_args(Token token);
	Expr parse_expr(Token token);
	Expr parse_primary_expr(Token token);
	Expr parse_expr_with_precedence(Token token, OpPrec prec);
	OpType get_op_type_by_token_type(Token token);
	OpPrec get_prec_by_op_type(OpType op_type);
	bool is_op(Token token);
};