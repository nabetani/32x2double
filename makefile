run: test
	./test

test: main.cpp d2_32x2.h makefile
	g++-13 -Wall -O2 -std=c++20 -o test main.cpp