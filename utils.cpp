#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include "utils.hpp"
#include "colors.hpp"

std::string Utils::read_file(std::string filepath) {
	std::fstream file(filepath, std::ios::in);
	if (!file) {
		std::cerr << "Error: file don't exists." << std::endl;
		exit(1);	
	}

	file.seekg(0, std::ios::end);
	std::streampos size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buf(size);
	file.read(&buf[0], size);

	return std::string(buf.begin(), buf.end());
}

void Utils::error(std::string message) {
	std::cerr << Colors::RED << message << Colors::RESET << std::endl;
	exit(1);
}
