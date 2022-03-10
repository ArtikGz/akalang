CC=g++
FLAGS=-Wall -Wextra -g -std=c++20 -O3

build: src/*.cpp src/*.hpp
	$(CC) $(FLAGS) -o main src/*.cpp