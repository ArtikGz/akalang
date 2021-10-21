#pragma once
#include <vector>
#include <map>
#include <stack>
#include "parser.hpp"
#include "lexer.hpp"

typedef struct {
	int rbp_offset;
	VarType type;
} Var_Declared;

typedef struct {
	int rbp_offset;
	std::map<std::string, Var_Declared> var_declare;
} Shared_Info;

// Registers order for function parameters
const std::vector<std::string> x64regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
const std::vector<std::string> x32regs = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
const std::vector<std::string> x16regs = {"di", "si", "dx", "cx", "r8w", "r9w"};
const std::vector<std::string> x8regs = {"dil", "sil", "dh", "ch", "r8b", "r9b"};
const std::string BUILTIN_PATH = "./builtin/";

class Compiler {
private:
	std::vector<Statement*> instructions;
	// Global function register
	std::map<std::string, std::vector<VarType>> global_function_register; 

	// Hardcoded strings
	std::vector<std::string> string_data_segment;

	void inc_rbp_offset(int& rbp_offset, VarType data_type);
	std::string get_reg_by_data_type_and_counter(int& counter, VarType data_type);
	std::string get_data_size_by_data_type(VarType data_type);
	std::string get_return_reg_by_data_type(VarType data_type);

public:
	Compiler(std::vector<Statement*> instructions);
	std::string compile_statement(Statement* stmt, Shared_Info& si);
	std::string compile_function(Statement* function);
	std::string compile_return(Statement* stmt, Shared_Info& si);
	std::string compile_var(Statement* stmt, Shared_Info& si);
	std::string compile_var_reasignation(Statement* stmt, Shared_Info& si);
	std::string compile_expr(Expr* expr, Shared_Info& si);
	std::string compile_func_call(Expr* expr, Shared_Info& si);
	void compile_op_tree(Expr* expr, std::stack<Expr*>& expr_stack, std::stack<OpType>& op_stack);
	std::string compile_op(Expr* expr, Shared_Info& si);
	std::string compile_operation(OpType type);
	std::string compile_boolean(Expr* expr);
	std::string compile_number(Expr* expr);
	std::string compile_string(Expr* expr);
	std::string compile_var_read(Expr* expr, Shared_Info& si);
	std::string compile_program();
	std::string build_data_segment();
	std::string build_bss_segment();
	std::string compile_builtin();
};