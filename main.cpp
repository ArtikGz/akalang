#include <iostream>
#include <fstream>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include "compiler.hpp"
#include "preprocessor.hpp"

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Syntax: " << argv[0] << " <filename>" << std::endl;
		exit(1);
	}

	std::string filename = argv[1];
	std::string preprocessed_file = Preprocessor::preprocess_includes(filename);

	Lexer lex = Lexer::from_content(preprocessed_file);

	// This is here for debugging purposes
	for (Token token: lex.get_tokens()) {
		std::cout << "Token[" << token.get_type() << "]" << "(" << token.get_value() << ")" << std::endl;
	}

	Parser parser = Parser(&lex);
	std::vector<Statement*> statements = parser.parse_code();

	Compiler compiler = Compiler(statements);
	std::string program = compiler.compile_program();
	std::cout << program << std::endl;
	std::ofstream file("main.asm");
	file << program;
	file.close();

	system("nasm -f elf64 -o main.o main.asm");
	system("ld -o main.out main.o");
	system("rm main.asm");
	system("rm main.o");

	return 0;
}