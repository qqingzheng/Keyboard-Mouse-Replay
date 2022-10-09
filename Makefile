all: recorder.exe
recorder.exe:
	g++ recorder.cpp -o recorder.exe
test:
	recorder