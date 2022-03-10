#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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
	std::vector<std::string> filenames;

	std::vector<Token> tokens;
	Preprocessor::preprocess_includes(filename, filenames, tokens);

	std::unique_ptr<Lexer> lex = std::make_unique<Lexer>();
	lex->set_tokens(tokens);

	Parser parser = Parser(std::move(lex));
	std::vector<std::shared_ptr<Statement>> statements = parser.parse_code();

	Compiler compiler = Compiler(statements);
	std::string program = compiler.compile_program();

	std::ofstream file("main.asm");
	file << program;
	file.close();

	system("nasm -f elf64 -o main.o main.asm");
	system("ld -o main.out main.o");
	system("rm main.asm");
	system("rm main.o");

	return 0;
}
