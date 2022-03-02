#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "token.hpp"
#include "lexer.hpp"

typedef struct VarType VarType;
typedef struct Func_Arg Func_Arg;
typedef struct Expr Expr;
typedef struct Func_Def Func_Def;
typedef struct Var_Asign Var_Asign;
typedef struct Ret Ret;
typedef struct If If;
typedef struct Op Op;
typedef struct Var_Read Var_Read;
typedef struct Statement Statement;

typedef enum {
	VAR_TYPE_INT,
	VAR_TYPE_LONG,
	VAR_TYPE_CHAR,
	VAR_TYPE_BOOL,
	VAR_TYPE_ANY, // used for syscalls
	VAR_TYPE_COUNTER
} VarTypeT;

struct VarType {
	size_t stars; // counter of pointers
	VarTypeT type;
};

typedef enum {
	STMT_TYPE_FUNCTION_DECLARATION,
	STMT_TYPE_VAR_REASIGNATION,
	STMT_TYPE_RETURN,
	STMT_TYPE_EXPR,
	STMT_TYPE_VAR_DECLARATION,
	STMT_TYPE_IF,
	STMT_TYPE_WHILE,
	STMT_TYPE_COUNTER
} StmtType;

typedef enum {
	OP_TYPE_ADD,
	OP_TYPE_SUB,
	OP_TYPE_DIV,
	OP_TYPE_MOD,
	OP_TYPE_MUL,
	OP_TYPE_LT,
	OP_TYPE_GT,
	OP_TYPE_EQ,
	OP_TYPE_NEQ,
	OP_TYPE_LTE,
	OP_TYPE_COUNT
} OpType;

typedef enum {
	OP_PREC_0,
	OP_PREC_1,
	OP_PREC_2,
	OP_PREC_COUNT
} OpPrec;

struct Op {
	OpType type;
	std::shared_ptr<Expr> lhs;
	std::shared_ptr<Expr> rhs;
};

struct Var_Read {
	std::string var_name;
	size_t stars;
};

struct Func_Arg {
	VarType type;
	std::string name;
};

struct Func_Def {
	std::string name;
	VarType return_type;
	std::vector<std::shared_ptr<Func_Arg>> arguments;
	std::vector<std::shared_ptr<Statement>> body;
};

struct Func_Call {
	std::string name;
	std::vector<std::shared_ptr<Expr>> expr;
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

static_assert(EXPR_TYPE_COUNTER == 6, "Unhandled EXPR_TYPE_COUNTER on parser.hpp");

struct Expr {
	ExprType type;
	std::shared_ptr<Func_Call> func_call;
	bool boolean;
	int number;
	Var_Read var_read;
	std::string string;
	std::shared_ptr<Op> op;
};

struct Var_Asign {
	std::string name;
	VarType type;
	std::shared_ptr<Expr> value;
};

struct Ret {
	std::shared_ptr<Expr> expr;
};

struct If {
	std::shared_ptr<Expr> condition;
	std::vector<std::shared_ptr<Statement>> then;
	std::vector<std::shared_ptr<Statement>> elsse;
};

struct While {
	std::shared_ptr<Expr> condition;
	std::vector<std::shared_ptr<Statement>> block;
};

struct Statement {
	StmtType type;
	std::shared_ptr<Func_Def> fnc;
	std::shared_ptr<Var_Asign> var;
	std::shared_ptr<Expr> expr;
	std::shared_ptr<If> iif;
  std::shared_ptr<While> whilee;
};

class Parser {
private:
	std::vector<Token> tokens;
	std::unique_ptr<Lexer> lexer;

public:
	Parser(std::unique_ptr<Lexer>&& lexer);
	std::shared_ptr<Statement> parse_function();
	std::vector<std::shared_ptr<Statement>> parse_block();
	std::vector<std::shared_ptr<Func_Arg>> parse_fnc_arguments(Token name_token);

	/**
	 * @brief Get akalang type from string
	 * 
	 * @param stars number of pointers before type
	 * @param val string with the string representation of the type
	 * @return VarType 
	 */
	VarType get_type_from_string(size_t stars, std::string val);

	/**
	 * @brief count number of pointer stars consecutive
	 * 
	 * @return size_t stars counted
	 */
	size_t count_stars();

	std::vector<std::shared_ptr<Statement>> parse_code();
	std::shared_ptr<Statement> parse_name();
	std::shared_ptr<Statement> parse_return();
	std::shared_ptr<Statement> parse_if();
	std::shared_ptr<Statement> parse_while();
	std::shared_ptr<Statement> parse_var_reasignation(Token name);
	std::shared_ptr<Statement> parse_var();
	std::shared_ptr<Func_Call> parse_func_call(Token name);
	std::vector<std::shared_ptr<Expr>> parse_func_call_args(Token token);
	std::shared_ptr<Expr> parse_expr(Token token);
	std::shared_ptr<Expr> parse_primary_expr(Token token);
	std::shared_ptr<Expr> parse_expr_with_precedence(Token token, OpPrec prec);
	OpType get_op_type_by_token_type(Token token);
	OpPrec get_prec_by_op_type(OpType op_type);
	bool is_op(Token token);
};
