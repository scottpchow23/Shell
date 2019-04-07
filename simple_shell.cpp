#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "Tokenizer.hpp"
#define MAX_LINE_SIZE 512
#define MAX_TOKEN_SIZE 32
#define STD_OUT 1
#define STD_IN 0

void parseInputLine(const char * inputLine);
void debugTokenizer(std::vector<Token> tokens);

int main(int argc, char *argv[]) {
  // bool output = true;
  std::string inputLine = std::string();
  char *input = new char[512];

  while(1) {
    std::cout << "shell:";

    char *eof = fgets(input, MAX_LINE_SIZE, stdin);
    if (eof == NULL) {
      break;
    } else {
      parseInputLine(input);
    }
    int status;
    pid_t background = waitpid(-1, &status, WNOHANG);
    if (background > 0) {
      std::cout << "[" << background <<"] completed in the background with status code: " << status << std::endl;
    }
  }

  return 0;
}

void lsExample1();
void lsExample2();
void lsExample3();
void grepExample();
void catBackgroundExample();

void parseInputLine(const char * inputLine) {
  std::string input = std::string(inputLine);
  input[input.length() - 1] = 0;
  std::vector<Token> tokens = tokenize(input);
  debugTokenizer(tokens);

  if (strcmp(inputLine,"ls\n") == 0) {
    lsExample1();
  } else if (strcmp(inputLine, "ls > test.txt\n") == 0) {
    lsExample2();
  } else if (strcmp(inputLine, "ls | cat\n") == 0) {
    lsExample3();
  } else if (strcmp(inputLine, "grep hun < test2.txt\n") == 0) {
    grepExample();
  } else if (strcmp(inputLine, "cat test.txt &\n") == 0) {
    catBackgroundExample();
  }
}


// test cases:
//     hi  there |   what> is<good&& my||||dude
//  || | < >
// hi|there|what&<>
void debugTokenizer(std::vector<Token> tokens) {
  std::cout << "[Tokens]:";
  for (Token token : tokens) {
    switch (token.type)
    {
      case Word:
        std::cout << " WORD ";
        break;
      case LT:
        std::cout << " LT ";
        break;
      case GT:
        std::cout << " GT ";
        break;
      case Pipe:
        std::cout << " Pipe ";
        break;
      case Amp:
        std::cout << " Amp ";
        break;
      default:
        break;
    }
  }
  std::cout << std::endl;

  std::cout << "[Values]:";
  for (Token token : tokens) {
    std::cout << " [" << token.value <<"] ";
  }
  std::cout << std::endl;
}

// runs ls .
void lsExample1() {
  pid_t childPID = fork();
  int status;

  if (childPID == 0) {
    // child process
    char * ls_args[2];
    ls_args[0] = "/bin/ls";
    ls_args[1] = 0;
    
    execve("/bin/ls", ls_args, 0);
  } else {
    // parent process
    waitpid(childPID, &status, 0);
    std::cout << "Child process " << childPID <<" returned with status code: " << status << std::endl;
  }
}

// runs ls . > test.txt
void lsExample2() {
  int status;
  if (fork()) {
    // parent
    waitpid(-1, &status, 0);
  } else {
    // child
    char * ls_args[2];
    ls_args[0] = "/bin/ls";
    ls_args[1] = 0;
    
    int outfile = open("test.txt", O_WRONLY);

    dup2(outfile, 1);

    close(outfile);

    execve("/bin/ls", ls_args, 0);
  }
}

// runs ls | cat
void lsExample3() {
  int status;
  if (fork()) {
    // parent
    waitpid(-1, &status, 0);
  } else {
    // child
    char * ls_args[2];
    ls_args[0] = "/bin/ls";
    ls_args[1] = 0;

    char * cat_args[1];
    cat_args[0] = 0;

    int fileDescriptors[2];
    pipe(fileDescriptors);
    int secondStatus;

    if (fork()) {
      // parent, handling cat (the last command)
      waitpid(-1, &secondStatus, 0);
      dup2(fileDescriptors[0], STD_IN);
      close(fileDescriptors[0]);
      close(fileDescriptors[1]);
      execve("/bin/cat", cat_args, 0);
    } else {
      // child, handling ls . (the first command)
      dup2(fileDescriptors[1], STD_OUT);
      close(fileDescriptors[0]);
      close(fileDescriptors[1]);
      execve("/bin/ls", ls_args, 0);
    }
  }
}

// runs grep hun < test2.txt
void grepExample() {
  int status;
  if (fork()) {
    // parent
    waitpid(-1, &status, 0);
  } else {
    // child
    char *grep_args[3];
    grep_args[0] = "/usr/bin/grep";
    grep_args[1] = "hun";
    grep_args[2] = 0;
    int infile = open("test2.txt", O_RDONLY);
    dup2(infile, STD_IN);
    close(infile);

    execve("/usr/bin/grep", grep_args, 0);
  }
}

// runs cat test.txt &
void catBackgroundExample() {
  int status;
  if (fork()) {
    
  } else {
    char *cat_args[3];
    cat_args[0] = "/bin/cat";
    cat_args[1] = "test.txt";
    cat_args[2] = 0;

    execve("/bin/cat", cat_args, 0);
  }
}