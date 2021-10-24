#include <iostream>
#include <sstream>
#include "compiler.hpp"

Compiler::Compiler(std::vector<Statement*> instructions) : instructions(instructions) {}

std::string Compiler::compile_program() {
	std::string program = "[bits 64]\nsegment .text\n\tglobal _start\n_start:\n\tcall main\n\tmov rdi, rax\n\tmov rax, 60\n\tsyscall\n";
	program += compile_builtin();

	for (Statement* stmt: instructions) {
		program += compile_function(stmt);
	}
	program += build_data_segment();
	program += build_bss_segment();

	return program;
}

std::string Compiler::compile_builtin() {
	std::string builtin_functions;
	std::vector<std::string> source_list {"print.asm", "len.asm", "println.asm", "input.asm", "socket.asm",
		"connect.asm", "printint.asm", "atoi.asm", "cleaninputbuf.asm", "rminputstrnl.asm", "printbool.asm"};
	for (const std::string& source: source_list) {
		builtin_functions += Utils::read_file(BUILTIN_PATH + source);
	}

	global_function_register["print"] = std::vector<VarType> {VAR_TYPE_STR}; // print.asm
	global_function_register["len"] = std::vector<VarType> {VAR_TYPE_STR}; // len.asm
	global_function_register["println"] = std::vector<VarType> {VAR_TYPE_STR}; // println.asm
	global_function_register["input"] = std::vector<VarType> {VAR_TYPE_STR}; // input.asm
	global_function_register["socket"] = std::vector<VarType> {VAR_TYPE_INT, VAR_TYPE_INT, VAR_TYPE_INT}; // socket.asm
	global_function_register["connect"] = std::vector<VarType> {VAR_TYPE_INT}; // connect.asm
	global_function_register["printint"] = std::vector<VarType> {VAR_TYPE_INT}; // printint.asm
	global_function_register["atoi"] = std::vector<VarType> {VAR_TYPE_STR}; // atoi.asm
	global_function_register["cleaninputbuf"] = std::vector<VarType> {}; // cleaninputbuf.asm
	global_function_register["rminputstrnl"] = std::vector<VarType> {}; // rminputstrnl.asm
	global_function_register["printbool"] = std::vector<VarType> {VAR_TYPE_BOOL}; // printbool.asm

	return builtin_functions;
}

std::string Compiler::compile_function(Statement* function) {
	Shared_Info si;
	std::stringstream body;
	si.rbp_offset = 0;
	si.if_counter = 0;
	si.while_counter = 0;
	if (function->fnc->arguments.size() > 6) {
		Utils::error("No more than 6 arguments on functions are allowed.");
	}

	std::vector<VarType> data_types;
	int param_counter = 0;
	for (Func_Arg* arg: function->fnc->arguments) {
		inc_rbp_offset(si.rbp_offset, arg->type);
		std::string reg = get_reg_by_data_type_and_counter(param_counter, arg->type);
		std::string data_size = get_data_size_by_data_type(arg->type);
		body << "\tmov " << data_size <<  " [rbp - " << si.rbp_offset << "], " << reg << "\n";
		si.var_declare[arg->name] = {.rbp_offset = si.rbp_offset, .type = arg->type};
		data_types.push_back(arg->type);
		param_counter++;
	}
	global_function_register[function->fnc->name] = data_types;

	for (Statement* stmt: function->fnc->body) {
		body << compile_statement(stmt, si);
	}

	std::stringstream compiled_function;
	compiled_function << function->fnc->name << ":\n\tpush rbp\n\tmov rbp, rsp\n\tsub rsp, " << si.rbp_offset << "\n";
	compiled_function << body.str();
	compiled_function << ".retpoint:\n\tadd rsp, " << si.rbp_offset << "\n\tpop rbp\n\tret\n";
	delete function;

	return compiled_function.str();
}

std::string Compiler::compile_statement(Statement* stmt, Shared_Info& si) {
	static_assert(STMT_TYPE_COUNTER == 7, "Unhandled STMT_TYPE_COUNTER on compile_statement on compiler.cpp");
	switch (stmt->type) {
		case STMT_TYPE_EXPR: return compile_expr(stmt->expr, si);
		case STMT_TYPE_RETURN: return compile_return(stmt, si);
		case STMT_TYPE_VAR_DECLARATION: return compile_var(stmt, si);
		case STMT_TYPE_VAR_REASIGNATION: return compile_var_reasignation(stmt, si);
		case STMT_TYPE_IF: return compile_if(stmt, si);
		case STMT_TYPE_WHILE: return compile_while(stmt, si);
		default: Utils::error("Unknown expression");
	}
}

std::string Compiler::compile_while(Statement* stmt, Shared_Info& si) {
	std::stringstream ss;
	int actual_while = si.while_counter++;
	ss << ".WHILE" << actual_while << ":\n";
	ss << compile_expr(stmt->whilee->condition, si);
	ss << "\tcmp eax, 0\n\tje .ENDWHILE" << actual_while << "\n";
	for (Statement* stmts: stmt->whilee->block) {
		ss << compile_statement(stmts, si);
	}

	ss << "\tjmp .WHILE" << actual_while << "\n";
	ss << ".ENDWHILE" << actual_while << ":\n";

	return ss.str();
}

std::string Compiler::compile_if(Statement* stmt, Shared_Info& si) {
	std::stringstream ss;
	ss << compile_expr(stmt->iif->condition, si);
	ss << "\tcmp eax, 0\n\tje .ELSE" << si.if_counter << "\n";
	for (Statement* stmts: stmt->iif->then) {
		ss << compile_statement(stmts, si);
	}
	ss << "\tjmp .ENDIF" << si.if_counter << "\n";

	ss << ".ELSE" << si.if_counter << ":\n";
	if (!stmt->iif->elsse.empty()) {
		for (Statement* stmts: stmt->iif->elsse) {
			ss << compile_statement(stmts, si);
		}
	}
	ss << ".ENDIF" << si.if_counter++ << ":\n";

	return ss.str();
}

std::string Compiler::compile_expr(Expr* expr, Shared_Info& si) {
	static_assert(EXPR_TYPE_COUNTER == 6, "Unhandled EXPR_TYPE_COUNTER in compiler_expr on compiler.cpp");
	switch (expr->type) {
		case EXPR_TYPE_FUNC_CALL: return compile_func_call(expr, si);
		case EXPR_TYPE_LITERAL_BOOL: return compile_boolean(expr);
		case EXPR_TYPE_LITERAL_NUMBER: return compile_number(expr);
		case EXPR_TYPE_LITERAL_STRING: return compile_string(expr);
		case EXPR_TYPE_VAR_READ: return compile_var_read(expr, si);
		case EXPR_TYPE_OP: return compile_op(expr, si);
		default: Utils::error("Unknown expression");
	}
}

void Compiler::compile_op_tree(Expr* expr, std::stack<Expr*>& expr_stack, std::stack<OpType>& op_stack) {
	if (expr->type == EXPR_TYPE_OP) {
		op_stack.push(expr->op->type);
		compile_op_tree(expr->op->rhs, expr_stack, op_stack);
		compile_op_tree(expr->op->lhs, expr_stack, op_stack);
	} else {
		expr_stack.push(expr);
	}
}

std::string Compiler::compile_op(Expr* expr, Shared_Info& si) {
	std::stack<Expr*> expr_stack;
	std::stack<OpType> op_stack;
	compile_op_tree(expr, expr_stack, op_stack);

	std::stringstream ss;
	Expr* cexpr = expr_stack.top();
	expr_stack.pop();
	ss << compile_expr(cexpr, si) << "\tmov ebx, eax\n";

	while(!op_stack.empty()) {
		cexpr = expr_stack.top();
		expr_stack.pop();
		ss << compile_expr(cexpr, si) << compile_operation(op_stack.top());
		op_stack.pop();
	}

	ss << "\tmov eax, ebx\n";
	return ss.str();
}

std::string Compiler::compile_operation(OpType type) {
	static_assert(OP_TYPE_COUNT == 8, "Unhandled OP_TYPE_COUNT on compile_operation() at compiler.cpp");
	switch (type) {
		case OP_TYPE_ADD:
			return "\tadd ebx, eax\n";

		case OP_TYPE_SUB:
			return "\tsub ebx, eax\n";

		case OP_TYPE_DIV: 
		 	return "\tmov ecx, eax\n"
				   "\tmov eax, ebx\n"
				   "\tmov ebx, ecx\n"
				   "\tpush rdx\n"
				   "\txor edx, edx\n"
				   "\tidiv ebx\n"
				   "\tmov ebx, eax\n"
				   "\tpop rdx\n";
	 
		case OP_TYPE_MOD: 
		 	return "\tmov ecx, eax\n"
				   "\tmov eax, ebx\n"
				   "\tmov ebx, ecx\n"
				   "\tpush rdx\n"
				   "\txor edx, edx\n"
				   "\tidiv ebx\n"
				   "\tmov ebx, edx\n"
				   "\tpop rdx\n";

		case OP_TYPE_MUL:
			return "\timul ebx, eax\n";

		case OP_TYPE_LT:
			return "\tcmp ebx, eax\n"
				   "\tsetl ah\n"
				   "\tmovzx ebx, ah\n";

		case OP_TYPE_GT:
			return "\tcmp ebx, eax\n"
				   "\tsetg ah\n"
				   "\tmovzx ebx, ah\n";

		case OP_TYPE_EQ:
			return "\tcmp ebx, eax\n"
				   "\tsete ah\n"
				   "\tmovzx ebx, ah\n";

		default: Utils::error("Unknown operation: " + type);
	}
}

std::string Compiler::compile_var(Statement* stmt, Shared_Info& si) {
	std::stringstream ss;
	if (si.var_declare.count(stmt->var->name) != 0) {
		Utils::error("Variable already declared before: " + stmt->var->name);
	}

	ss << compile_expr(stmt->var->value, si);
	inc_rbp_offset(si.rbp_offset, stmt->var->type);
	ss << "\tmov " << get_data_size_by_data_type(stmt->var->type) << "[rbp - " << si.rbp_offset << "], " << get_return_reg_by_data_type(stmt->var->type) << "\n";
	si.var_declare[stmt->var->name] = {.rbp_offset = si.rbp_offset, .type = stmt->var->type};
	return ss.str();
}

std::string Compiler::compile_var_reasignation(Statement* stmt, Shared_Info& si) {
	std::stringstream ss;
	if (si.var_declare.count(stmt->var->name) == 0) {
		Utils::error("Trying to reasign an undeclared variable: " + stmt->var->name);
	}
	Var_Declared vd = si.var_declare[stmt->var->name];

	ss << compile_expr(stmt->var->value, si);
	ss << "\tmov [rbp - " << vd.rbp_offset << "], "  << get_return_reg_by_data_type(vd.type) << "\n";
	return ss.str();
}

std::string Compiler::compile_var_read(Expr* expr, Shared_Info& si) {
	std::stringstream ss;
	if (si.var_declare.count(expr->var_read) == 0) {
		Utils::error("Undefined variable: " + expr->var_read);
	}
	Var_Declared vd = si.var_declare[expr->var_read];

	ss << "\tmov " << get_return_reg_by_data_type(vd.type) << ", [rbp - " << vd.rbp_offset << "]\n";
	return ss.str();
}

std::string Compiler::compile_boolean(Expr* expr) {
	std::string compiled_boolean;
	if (expr->boolean) {
		compiled_boolean = "\tmov eax, 1\n";
	} else {
		compiled_boolean = "\tmov eax, 0\n";
	}

	return compiled_boolean;
}

std::string Compiler::compile_number(Expr* expr) {
	std::stringstream compiled_number;
	compiled_number << "\tmov eax, " ;
	compiled_number << expr->number;
	compiled_number << "\n";

	return compiled_number.str();
}

std::string Compiler::compile_string(Expr* expr) {
	int data_identifier = string_data_segment.size();
	std::string compiled_string;
	char tmp[6] = {0};
	for (int i = 0; i < expr->string.length(); i++) {
		sprintf(tmp, "0x%02x,", expr->string[i]);
		compiled_string += tmp;
	}
	compiled_string += "0x00\n"; 
	string_data_segment.push_back(compiled_string);

	char str[3] = {0};
	sprintf(str, "V%d", data_identifier);

	return "\tmov rax, " + std::string(str) + "\n";
}

std::string Compiler::compile_func_call(Expr* expr, Shared_Info& si) {
	std::stringstream compiled_func_call;
	if (expr->func_call->expr.size() > 6) {
		Utils::error("Max number of params allowed in functions: 6");
	}

	// Check if function is declared
	int func = global_function_register.count(expr->func_call->name);
	if (func == 0) {
		Utils::error("Undefined function: " + expr->func_call->name);
	}

	std::vector<VarType> data_type = global_function_register[expr->func_call->name];
	if (expr->func_call->expr.size() != data_type.size()) {
		Utils::error("Unexpected number of arguments on function call");
	}

	int param_counter = 0;

	// Calc registers used by functions
	std::vector<std::string> func_regs;
	std::vector<std::string> rest_regs;
	// First parse the func calls for avoiding problems with the registers
	for (Expr* expr: expr->func_call->expr) {
		if (expr->type == EXPR_TYPE_FUNC_CALL) {
			func_regs.push_back(get_reg_by_data_type_and_counter(param_counter, data_type[param_counter]));
		} else {
			rest_regs.push_back(get_reg_by_data_type_and_counter(param_counter, data_type[param_counter]));
		}

		param_counter++;
	}

	param_counter = 0;
	int index_counter = 0;
	for (Expr* expr: expr->func_call->expr) {
		if (expr->type == EXPR_TYPE_FUNC_CALL) {
			compiled_func_call << compile_expr(expr, si);
			compiled_func_call << "\tmov " + func_regs[index_counter++] + ", " +  get_return_reg_by_data_type(data_type[param_counter]) + "\n";
		}

		param_counter++;
	}

	param_counter = 0;
	index_counter = 0;
	for (Expr* expr: expr->func_call->expr) {
		if (expr->type != EXPR_TYPE_FUNC_CALL) {
			compiled_func_call << compile_expr(expr, si);
			compiled_func_call << "\tmov " + rest_regs[index_counter++] + ", " + get_return_reg_by_data_type(data_type[param_counter]) + "\n";
		}

		param_counter++;
	}

	compiled_func_call << "\tcall " + expr->func_call->name + "\n";
	return compiled_func_call.str();
}

std::string Compiler::compile_return(Statement* stmt, Shared_Info& si) {
	std::stringstream compiled_return;
	compiled_return << compile_expr(stmt->expr, si) << "\tjmp .retpoint\n";

	return compiled_return.str();
}

void Compiler::inc_rbp_offset(int& rbp_offset, VarType data_type) {
	static_assert(VAR_TYPE_COUNTER == 4, "Unhandled VAR_TYPE_COUNTER on inc_rbp_offset on compiler.cpp");
	switch (data_type) {
		case VAR_TYPE_LONG: rbp_offset += 8; break;
		case VAR_TYPE_STR: rbp_offset += 8; break;
		case VAR_TYPE_INT: rbp_offset += 4; break;
		case VAR_TYPE_BOOL: rbp_offset += 4; break;
		default: Utils::error("Unknown datatype");
	}
}

std::string Compiler::get_reg_by_data_type_and_counter(int& counter, VarType data_type) {
	static_assert(VAR_TYPE_COUNTER == 4, "Unhandled VAR_TYPE_COUNTER on get_reg_by_data_type_and_counter on compiler.cpp");
	switch (data_type) {
		case VAR_TYPE_LONG: return x64regs[counter];
		case VAR_TYPE_STR: return x64regs[counter];
		case VAR_TYPE_INT: return x32regs[counter];
		case VAR_TYPE_BOOL: return x32regs[counter];
		default: Utils::error("Unknown datatype");
	}
}

std::string Compiler::get_return_reg_by_data_type(VarType data_type) {
	static_assert(VAR_TYPE_COUNTER == 4, "Unhandled VAR_TYPE_COUNTER on get_reg_by_data_type_and_counter on compiler.cpp");
	switch (data_type) {
		case VAR_TYPE_LONG: return "rax";
		case VAR_TYPE_STR: return "rax";
		case VAR_TYPE_INT: return "eax";
		case VAR_TYPE_BOOL: return "eax";
		default: Utils::error("Unknown datatype");
	}
}

std::string Compiler::get_data_size_by_data_type(VarType data_type) {
	static_assert(VAR_TYPE_COUNTER == 4, "Unhandled VAR_TYPE_COUNTER on get_data_size_by_data_type an compiler.cpp");
	switch (data_type) {
		case VAR_TYPE_LONG: return "qword";
		case VAR_TYPE_STR: return "qword";
		case VAR_TYPE_INT: return "dword";
		case VAR_TYPE_BOOL: return "dword";
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
	compiled_data_segment << "\ttext_true db \"true\", 0x00\n";
	compiled_data_segment << "\ttext_false db \"false\", 0x00\n";

	return compiled_data_segment.str();
}

std::string Compiler::build_bss_segment() {
	std::stringstream compiled_bss_segment;
	compiled_bss_segment << "segment .bss\n";
	compiled_bss_segment << "\tinputstr resb 50\n";
	return compiled_bss_segment.str();
}
