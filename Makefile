all: recorder.exe
recorder.exe: bin/main.o
	g++ $< -o recorder.exe
bin/main.o:
	g++ main.cpp -c -o bin/main.o
test:
	recorder