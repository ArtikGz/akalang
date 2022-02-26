CC=g++
FLAGS=-Wall -Wextra -g -std=c++20

build: *.cpp *.hpp
	$(CC) $(FLAGS) -o main *.cpp