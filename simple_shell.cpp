#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#define MAX_LINE_SIZE 512
#define MAX_TOKEN_SIZE 32
#define STD_OUT 1
#define STD_IN 0

void parseInputLine(const char * inputLine);

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
      // std::cin >> inputLine;
      
      // std::cin.read(input, MAX_LINE_SIZE);
      
      parseInputLine(input);
      

    }
    
    std::cout << "Echo:" << input << std::endl;

  }

  return 0;
}

void lsExample1();
void lsExample2();
void lsExample3();

void parseInputLine(const char * inputLine) {
  if (strcmp(inputLine,"ls\n") == 0) {
    lsExample1();
  } else if (strcmp(inputLine, "ls > test.txt\n") == 0) {
    lsExample2();
  } else if (strcmp(inputLine, "ls | cat\n") == 0) {
    lsExample3();
  }
}

// runs ls .
void lsExample1() {
  pid_t childPID = fork();
  int status;

  if (childPID == 0) {
    // child process
    char * ls_args[2];
    ls_args[0] = ".";
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
    ls_args[0] = ".";
    ls_args[1] = 0;
    
    int outfile = open("test.txt", O_WRONLY);

    dup2(outfile, 1);

    close(outfile);

    execve("/bin/ls", ls_args, 0);
  }
}

// runs ls . | cat
void lsExample3() {
  int status;
  if (fork()) {
    // parent
    waitpid(-1, &status, 0);
  } else {
    // child
    char * ls_args[2];
    ls_args[0] = ".";
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