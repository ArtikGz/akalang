#include <iostream>
#include <sstream>
#include "compiler.hpp"

Compiler::Compiler(std::vector<Statement> instructions) : instructions(instructions) {}

std::string Compiler::compile_program() {
	std::string program = "[bits 64]\nsegment .text\n\tglobal _start\n_start:\n\tcall main\n\tmov rdi, rax\n\tmov rax, 60\n\tsyscall\n";
	program += compile_builtin();

	for (Statement stmt: instructions) {
		program += compile_function(stmt);
	}
	program += build_data_segment();

	return program;
}

std::string Compiler::compile_builtin() {
	std::string builtin_functions;
	std::vector<std::string> source_list {"print.asm", "len.asm", "println.asm"};
	for (const std::string& source: source_list) {
		builtin_functions += Utils::read_file(BUILTIN_PATH + source);
	}

	global_function_register["__BUILTIN_PRINT"] = std::vector<VarType> {VAR_TYPE_STR, VAR_TYPE_INT}; // print.asm
	global_function_register["len"] = std::vector<VarType> {VAR_TYPE_STR}; // len.asm
	global_function_register["println"] = std::vector<VarType> {VAR_TYPE_STR}; // println.asm

	return builtin_functions;
}

std::string Compiler::compile_function(Statement function) {
	Shared_Info si;
	std::stringstream body;
	si.rbp_offset = 0;
	if (function.as.fnc.arguments.size() > 6) {
		Utils::error("No more than 6 arguments on functions are allowed.");
	}

	std::vector<VarType> data_types;
	int param_counter = 0;
	for (Func_Arg arg: function.as.fnc.arguments) {
		inc_rbp_offset(si.rbp_offset, arg.type);
		std::string reg = get_reg_by_data_type_and_counter(param_counter, arg.type);
		std::string data_size = get_data_size_by_data_type(arg.type);
		body << "\tmov " << data_size <<  " [rbp - " << si.rbp_offset << "], " << reg << "\n";
		si.var_declare[arg.name] = {.rbp_offset = si.rbp_offset, .type = arg.type};
		data_types.push_back(arg.type);
	}
	global_function_register[function.as.fnc.name] = data_types;

	for (Statement stmt: function.as.fnc.body) {
		body << compile_statement(stmt, si);
	}

	std::stringstream compiled_function;
	compiled_function << function.as.fnc.name << ":\n\tpush rbp\n\tmov rbp, rsp\n\tsub rsp, " << si.rbp_offset << "\n";
	compiled_function << body.str();
	compiled_function << ".retpoint:\n\tadd rsp, " << si.rbp_offset << "\n\tpop rbp\n\tret\n";

	return compiled_function.str();
}

std::string Compiler::compile_statement(Statement stmt, Shared_Info& si) {
	static_assert(STMT_TYPE_COUNTER == 5, "Unhandled STMT_TYPE_COUNTER on compile_statement on compiler.cpp");
	switch (stmt.type) {
		case STMT_TYPE_EXPR: return compile_expr(stmt.as.expr, si);
		case STMT_TYPE_RETURN: return compile_return(stmt, si);
		case STMT_TYPE_VAR_DECLARATION: return compile_var(stmt, si);
		case STMT_TYPE_VAR_REASIGNATION: return compile_var_reasignation(stmt, si);
		default: Utils::error("Unknown expression");
	}
}

std::string Compiler::compile_expr(Expr expr, Shared_Info& si) {
	static_assert(EXPR_TYPE_COUNTER == 5, "Unhandled EXPR_TYPE_COUNTER in compiler_expr on compiler.cpp");
	switch (expr.type) {
		case EXPR_TYPE_FUNC_CALL: return compile_func_call(expr, si);
		case EXPR_TYPE_LITERAL_BOOL: return compile_boolean(expr);
		case EXPR_TYPE_LITERAL_NUMBER: return compile_number(expr);
		case EXPR_TYPE_LITERAL_STRING: return compile_string(expr);
		case EXPR_TYPE_VAR_READ: return compile_var_read(expr, si);
		default: Utils::error("Unknown expression");
	}
}

std::string Compiler::compile_var(Statement stmt, Shared_Info& si) {
	std::stringstream ss;
	if (si.var_declare.count(stmt.as.var.name) != 0) {
		Utils::error("Variable already declared before: " + stmt.as.var.name);
	}

	ss << compile_expr(stmt.as.var.value, si);
	inc_rbp_offset(si.rbp_offset, stmt.as.var.type);
	ss << "\tmov [rbp - " << si.rbp_offset << "], " << get_return_reg_by_data_type(stmt.as.var.type) << "\n";
	si.var_declare[stmt.as.var.name] = {.rbp_offset = si.rbp_offset, .type = stmt.as.var.type};
	return ss.str();
}

std::string Compiler::compile_var_reasignation(Statement stmt, Shared_Info& si) {
	std::stringstream ss;
	if (si.var_declare.count(stmt.as.var.name) == 0) {
		Utils::error("Trying to reasign an undeclared variable: " + stmt.as.var.name);
	}
	Var_Declared vd = si.var_declare[stmt.as.var.name];

	ss << compile_expr(stmt.as.var.value, si);
	ss << "\tmov [rbp - " << vd.rbp_offset << "], "  << get_return_reg_by_data_type(vd.type) << "\n";
	return ss.str();
}

std::string Compiler::compile_var_read(Expr expr, Shared_Info& si) {
	std::stringstream ss;
	if (si.var_declare.count(expr.as.var_read) == 0) {
		Utils::error("Undefined variable: " + expr.as.var_read);
	}
	Var_Declared vd = si.var_declare[expr.as.var_read];

	ss << "\tmov " << get_return_reg_by_data_type(vd.type) << ", [rbp - " << vd.rbp_offset << "]\n";
	return ss.str();
}

std::string Compiler::compile_boolean(Expr expr) {
	std::string compiled_boolean;
	if (expr.as.boolean) {
		compiled_boolean = "\tmov ah, 1\n";
	} else {
		compiled_boolean = "\tmov ah, 0\n";
	}

	return compiled_boolean;
}

std::string Compiler::compile_number(Expr expr) {
	std::stringstream compiled_number;
	compiled_number << "\tmov eax, " ;
	compiled_number << expr.as.number;
	compiled_number << "\n";

	return compiled_number.str();
}

std::string Compiler::compile_string(Expr expr) {
	int data_identifier = string_data_segment.size();
	std::string compiled_string;
	char tmp[6] = {0};
	for (int i = 0; i < expr.as.string.length(); i++) {
		sprintf(tmp, "0x%02x,", expr.as.string[i]);
		compiled_string += tmp;
	}
	compiled_string += "0x00\n"; 
	string_data_segment.push_back(compiled_string);

	char str[3] = {0};
	sprintf(str, "V%d", data_identifier);

	return "\tmov rax, " + std::string(str) + "\n";
}

std::string Compiler::compile_func_call(Expr expr, Shared_Info& si) {
	std::stringstream compiled_func_call;
	if (expr.as.func_call.expr.size() > 6) {
		Utils::error("Max number of params allowed in functions: 6");
	}

	// Check if function is declared
	int func = global_function_register.count(expr.as.func_call.name);
	if (func == 0) {
		Utils::error("Undefined function");
	}

	std::vector<VarType> data_type = global_function_register[expr.as.func_call.name];
	if (expr.as.func_call.expr.size() != data_type.size()) {
		Utils::error("Unexpected number of arguments on function call");
	}

	int param_counter = 0;

	// Calc registers used by functions
	std::vector<std::string> func_regs;
	std::vector<std::string> rest_regs;
	// First parse the func calls for avoiding problems with the registers
	for (Expr expr: expr.as.func_call.expr) {
		if (expr.type == EXPR_TYPE_FUNC_CALL) {
			func_regs.push_back(get_reg_by_data_type_and_counter(param_counter, data_type[param_counter]));
		} else {
			rest_regs.push_back(get_reg_by_data_type_and_counter(param_counter, data_type[param_counter]));
		}

		param_counter++;
	}

	param_counter = 0;
	int index_counter = 0;
	for (Expr expr: expr.as.func_call.expr) {
		if (expr.type == EXPR_TYPE_FUNC_CALL) {
			compiled_func_call << compile_expr(expr, si);
			compiled_func_call << "\tmov " + func_regs[index_counter++] + ", " +  get_return_reg_by_data_type(data_type[param_counter]) + "\n";
		}

		param_counter++;
	}

	param_counter = 0;
	index_counter = 0;
	for (Expr expr: expr.as.func_call.expr) {
		if (expr.type != EXPR_TYPE_FUNC_CALL) {
			compiled_func_call << compile_expr(expr, si);
			compiled_func_call << "\tmov " + rest_regs[index_counter++] + ", " + get_return_reg_by_data_type(data_type[param_counter]) + "\n";
		}

		param_counter++;
	}

	compiled_func_call << "\tcall " + expr.as.func_call.name + "\n";
	return compiled_func_call.str();
}

std::string Compiler::compile_return(Statement stmt, Shared_Info& si) {
	std::stringstream compiled_return;
	compiled_return << compile_expr(stmt.as.expr, si) << "\tjmp .retpoint\n";

	return compiled_return.str();
}

void Compiler::inc_rbp_offset(int& rbp_offset, VarType data_type) {
	static_assert(VAR_TYPE_COUNTER == 4, "Unhandled VAR_TYPE_COUNTER on inc_rbp_offset on compiler.cpp");
	switch (data_type) {
		case VAR_TYPE_LONG: rbp_offset += 8; break;
		case VAR_TYPE_STR: rbp_offset += 8; break;
		case VAR_TYPE_INT: rbp_offset += 4; break;
		case VAR_TYPE_BOOL: rbp_offset += 1; break;
		default: Utils::error("Unknown datatype");
	}
}

std::string Compiler::get_reg_by_data_type_and_counter(int& counter, VarType data_type) {
	static_assert(VAR_TYPE_COUNTER == 4, "Unhandled VAR_TYPE_COUNTER on get_reg_by_data_type_and_counter on compiler.cpp");
	switch (data_type) {
		case VAR_TYPE_LONG: return x64regs[counter];
		case VAR_TYPE_STR: return x64regs[counter];
		case VAR_TYPE_INT: return x32regs[counter];
		case VAR_TYPE_BOOL: return x8regs[counter];
		default: Utils::error("Unknown datatype");
	}
}

std::string Compiler::get_return_reg_by_data_type(VarType data_type) {
	static_assert(VAR_TYPE_COUNTER == 4, "Unhandled VAR_TYPE_COUNTER on get_reg_by_data_type_and_counter on compiler.cpp");
	switch (data_type) {
		case VAR_TYPE_LONG: return "rax";
		case VAR_TYPE_STR: return "rax";
		case VAR_TYPE_INT: return "eax";
		case VAR_TYPE_BOOL: return "ah";
		default: Utils::error("Unknown datatype");
	}
}

std::string Compiler::get_data_size_by_data_type(VarType data_type) {
	static_assert(VAR_TYPE_COUNTER == 4, "Unhandled VAR_TYPE_COUNTER on get_data_size_by_data_type an compiler.cpp");
	switch (data_type) {
		case VAR_TYPE_LONG: return "qword";
		case VAR_TYPE_STR: return "qword";
		case VAR_TYPE_INT: return "dword";
		case VAR_TYPE_BOOL: return "byte";
		default: Utils::error("Unknown datatype");
	}
}

std::string Compiler::build_data_segment() {
	std::stringstream compiled_data_segment;
	compiled_data_segment << "segment .data\n";
	int c = 0;
	for (const std::string& str: string_data_segment) {
		compiled_data_segment << "\tV" << c++ << " db " << str;
	}
	compiled_data_segment << "\tln db 0x0A\n";

	return compiled_data_segment.str();
}