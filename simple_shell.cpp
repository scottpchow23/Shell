#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "Tokenizer.hpp"
#include "Parser.hpp"
#define MAX_LINE_SIZE 512
#define MAX_TOKEN_SIZE 32
#define STD_OUT 1
#define STD_IN 0
#define DEBUG_TOKENIZER 0
#define DEBUG_EXECUCTOR 0


std::vector<Token> tokenizeInputLine(const char * inputLine);
std::shared_ptr<Parser::Input> parseForInput(std::vector<Token> tokens);
void execute(std::shared_ptr<Parser::Input> input);
void debugTokenizer(std::vector<Token> tokens);

int main(int argc, char *argv[]) {
  // bool output = true;
  char *input = new char[512];

  while(1) {
    std::cout << "shell:";

    char *eof = fgets(input, MAX_LINE_SIZE, stdin);
    if (eof == NULL) {
      break;
    } else {
      std::vector<Token> tokens = tokenizeInputLine(input);
      std::shared_ptr<Parser::Input> inputAST = parseForInput(tokens);
      if (tokens.size() > 1)
        execute(inputAST);
    }
    int status;
    pid_t background = waitpid(-1, &status, WNOHANG);
    if (background > 0) {
      std::cout << "[" << background <<"] completed in the background with status code: " << status << std::endl;
    }
  }

  return 0;
}

// void lsExample1();
// void lsExample2();
// void lsExample3();
// void grepExample();
// void catBackgroundExample();

std::vector<Token> tokenizeInputLine(const char * inputLine) {
  std::string input = std::string(inputLine);
  input[input.length() - 1] = 0;
  std::vector<Token> tokens = tokenize(input);
  if (DEBUG_TOKENIZER)
    debugTokenizer(tokens);

  return tokens;
}

std::shared_ptr<Parser::Input> parseForInput(std::vector<Token> tokens) {
  const auto& [parsed, inputAST] = parse(tokens);

  if (parsed) {
    if (DEBUG_PARSER)
      std::cout << "It parsed!" << std::endl;
  } else {
    if (DEBUG_PARSER)
      std::cout << "It didn't parse." << std::endl;
    std::cerr << "ERROR: Failed to parse command." << std::endl;
  }

  return inputAST;
}

void execute(std::shared_ptr<Parser::Input> input) {
  pid_t childPID = fork();
  int status;

  if (childPID) {
    // Parent process
    // If not backgrounded wait, otherwise move on
    if (!input->background) {
      waitpid(childPID, &status, 0);
    }
  } else {
    // Child Process


    std::shared_ptr<Parser::CommandSeq> commandSeq = input->cs;

    // while(commandSeq->right) {
    //   pid_t child = fork();
    //   int childStatus;
    //   if (child) {
    //     // Parent
    //     waitpid(child, &childStatus, 0);
    //     commandSeq = commandSeq->right;
    //   } else {
        
    //   }
    // }
    
    std::shared_ptr<Parser::Command> command = input->cs->left;

    std::vector<char *> args;
    for (std::shared_ptr<Parser::Word> word : command->argv) {
      args.push_back(&word->value.front());
    }

    if (command->in) {
      int infile = open(command->in->value.c_str(), O_RDONLY);
      dup2(infile, STD_IN);
      close(infile);
    }

    if (command->out) {
      int outfile = open(command->out->value.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
      dup2(outfile, STD_OUT);
      close(outfile);
    }
    
    args.push_back(nullptr);
    if (DEBUG_EXECUCTOR) {
      std::cout << "[Execve]: ";
      for (char * arg : args) {
        if (arg != nullptr)
          std::cout << arg << " ";
      }
      std::cout << std::endl;
    }
    execvp(args.at(0), &args.front());
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

// // runs ls .
// void lsExample1() {
//   pid_t childPID = fork();
//   int status;

//   if (childPID == 0) {
//     // child process
//     char * ls_args[2];
//     ls_args[0] = "/bin/ls";
//     ls_args[1] = 0;
    
//     execve("/bin/ls", ls_args, 0);
//   } else {
//     // parent process
//     waitpid(childPID, &status, 0);
//     std::cout << "Child process " << childPID <<" returned with status code: " << status << std::endl;
//   }
// }

// // runs ls . > test.txt
// void lsExample2() {
//   int status;
//   if (fork()) {
//     // parent
//     waitpid(-1, &status, 0);
//   } else {
//     // child
//     char * ls_args[2];
//     ls_args[0] = "/bin/ls";
//     ls_args[1] = 0;
    
//     int outfile = open("test.txt", O_WRONLY);

//     dup2(outfile, 1);

//     close(outfile);

//     execve("/bin/ls", ls_args, 0);
//   }
// }

// // runs ls | cat
// void lsExample3() {
//   int status;
//   if (fork()) {
//     // parent
//     waitpid(-1, &status, 0);
//   } else {
//     // child
//     char * ls_args[2];
//     ls_args[0] = "/bin/ls";
//     ls_args[1] = 0;

//     char * cat_args[1];
//     cat_args[0] = 0;

//     int fileDescriptors[2];
//     pipe(fileDescriptors);
//     int secondStatus;

//     if (fork()) {
//       // parent, handling cat (the last command)
//       waitpid(-1, &secondStatus, 0);
//       dup2(fileDescriptors[0], STD_IN);
//       close(fileDescriptors[0]);
//       close(fileDescriptors[1]);
//       execve("/bin/cat", cat_args, 0);
//     } else {
//       // child, handling ls . (the first command)
//       dup2(fileDescriptors[1], STD_OUT);
//       close(fileDescriptors[0]);
//       close(fileDescriptors[1]);
//       execve("/bin/ls", ls_args, 0);
//     }
//   }
// }

// // runs grep hun < test2.txt
// void grepExample() {
//   int status;
//   if (fork()) {
//     // parent
//     waitpid(-1, &status, 0);
//   } else {
//     // child
//     char *grep_args[3];
//     grep_args[0] = "/usr/bin/grep";
//     grep_args[1] = "hun";
//     grep_args[2] = 0;
//     int infile = open("test2.txt", O_RDONLY);
//     dup2(infile, STD_IN);
//     close(infile);

//     execve("/usr/bin/grep", grep_args, 0);
//   }
// }

// // runs cat test.txt &
// void catBackgroundExample() {
//   int status;
//   if (fork()) {
    
//   } else {
//     char *cat_args[3];
//     cat_args[0] = "/bin/cat";
//     cat_args[1] = "test.txt";
//     cat_args[2] = 0;

//     execve("/bin/cat", cat_args, 0);
//   }
// }