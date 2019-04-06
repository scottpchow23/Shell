#include <iostream>
#include <string>
#include <unistd.h>
#define MAX_LINE_SIZE 512
#define MAX_TOKEN_SIZE 32

void parseInputLine(std::string inputLine);

int main(int argc, char *argv[]) {
  // bool output = true;
  std::string inputLine = std::string();
  

  while(1) {
    std::cout << "shell:";

    if (std::cin.eof()) {
      break;
    } else {
      std::cin >> inputLine;
      
      parseInputLine(inputLine);
      

    }
    
    std::cout << "Echo:" << inputLine << std::endl;

  }

  return 0;
}

void parseInputLine(std::string inputLine) {

  if (inputLine == "ls") {
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

}