CC=g++
FLAGS=-Wall -Wextra -g -std=c++20

build: *.cpp
	$(CC) $(FLAGS) -o main *.cpp