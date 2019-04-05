CXX = g++
CXXFLAGS = -Wall -g -fsanitize=address



all: simple_shell
	$(CXX) $(CXXFLAGS) -o simple_shell simple_shell.cpp

clean:
	rm -rf *.o simple_shell simple_shell.dSYM 