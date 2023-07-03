all: compile run

compile:
	g++ main.cpp -o main.exe -lws2_32

run:
	main.exe