#pragma once
#include <vector>
#include <map>
#include <stack>
#include "parser.hpp"
#include "lexer.hpp"

#define VAR_TYPE(varType, starsC) (VarType) { \
										.stars = starsC, \
										.type = varType  \
									}

typedef struct {
	int rbp_offset;
	VarType type;
} Var_Declared;

typedef struct {
	int rbp_offset;
	int if_counter;
	int while_counter;
	std::map<std::string, Var_Declared> var_declare;
} Shared_Info;

// Registers order for function parameters
const std::vector<std::string> x64regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
const std::vector<std::string> x32regs = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
const std::vector<std::string> x16regs = {"di", "si", "dx", "cx", "r8w", "r9w"};
const std::vector<std::string> x8regs = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
const std::string BUILTIN_PATH = "./builtin/";

class Compiler {
private:
	std::vector<std::shared_ptr<Statement>> instructions;
	// Global function register
	std::map<std::string, std::vector<VarType>> global_function_register; 

	// Hardcoded strings
	std::vector<std::string> string_data_segment;

	void inc_rbp_offset(int& rbp_offset, VarType data_type);
	std::string get_reg_by_data_type_and_counter(int& counter, VarType data_type);
	std::string get_data_size_by_data_type(VarType data_type);
	std::string get_return_reg_by_data_type(VarType data_type);

public:
	Compiler(std::vector<std::shared_ptr<Statement>> instructions);
	std::string compile_statement(std::shared_ptr<Statement> stmt, Shared_Info& si);
	std::string compile_function(std::shared_ptr<Statement> function);
	std::string compile_return(std::shared_ptr<Statement> stmt, Shared_Info& si);
	std::string compile_var(std::shared_ptr<Statement> stmt, Shared_Info& si);
	std::string compile_var_reasignation(std::shared_ptr<Statement> stmt, Shared_Info& si);
	std::string compile_if(std::shared_ptr<Statement> stmt, Shared_Info& si);
	std::string compile_while(std::shared_ptr<Statement> stmt, Shared_Info& si);
	std::string compile_expr(std::shared_ptr<Expr> expr, Shared_Info& si);
	std::string compile_func_call(std::shared_ptr<Expr> expr, Shared_Info& si);
	void compile_op_tree(std::shared_ptr<Expr> expr, std::stack<std::shared_ptr<Expr>>& expr_stack, std::stack<OpType>& op_stack);
	std::string compile_op(std::shared_ptr<Expr> expr, Shared_Info& si);
	std::string compile_operation(OpType type);
	std::string compile_boolean(std::shared_ptr<Expr> expr);
	std::string compile_number(std::shared_ptr<Expr> expr);
	std::string compile_string(std::shared_ptr<Expr> expr);
	std::string compile_var_read(std::shared_ptr<Expr> expr, Shared_Info& si);
	std::string compile_program();
	std::string build_data_segment();
	std::string build_bss_segment();
	std::string compile_builtin();
};
