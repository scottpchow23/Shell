CXX = g++
CXXFLAGS = -Wall -g -fsanitize=address



all: simple_shell.o Tokenizer.o
	$(CXX) $(CXXFLAGS) -o simple_shell Tokenizer.o simple_shell.o

simple_shell.o: Tokenizer.o
	$(CXX) $(CXXFLAGS) -c simple_shell.cpp

Tokenizer.o: 
	$(CXX) $(CXXFLAGS) -c Tokenizer.cpp

clean:
	rm -rf *.o simple_shell simple_shell.dSYM 