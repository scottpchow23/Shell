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

bool printShell = true;

void sig_trap(int sig) {
  // std::cerr << "Caught signal " << sig << "from child process." << std::endl;
  std::cout << std::endl;
  std::cout << "shell:";
  printShell = false;
  std::cout.flush();
}

int main(int argc, char *argv[]) {
  // bool output = true;
  char *input = new char[512];
  signal(SIGINT, &sig_trap);

  while(1) {
    if (printShell){
      std::cout << "shell:";
    } else {
      printShell = true;
    }

    char *eof = fgets(input, MAX_LINE_SIZE, stdin);
    if (eof == NULL) {
      std::cout << std::endl;
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

void executeCommand(std::shared_ptr<Parser::Command> command, int input = -1, int output = -1) {
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

  if (input > 0) {
    dup2(input, STD_IN);
  }

  if (output > 0) {
    dup2(output, STD_OUT);
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

void executeCommandSeq(std::shared_ptr<Parser::CommandSeq> commandSeq, int redirectedInput = -1) {
  if (commandSeq->right) {
    int pipefd[2];
    pipe(pipefd);
    pid_t child = fork();

    if (child) {
      int status;
      close(pipefd[1]);
      waitpid(child, &status, 0);
      executeCommandSeq(commandSeq->right, pipefd[0]);
    } else {
      close(pipefd[0]);
      executeCommand(commandSeq->left, redirectedInput, pipefd[1]);
    }
  } else {
    executeCommand(commandSeq->left, redirectedInput);
  }
}

void execute(std::shared_ptr<Parser::Input> input) {
  pid_t childPID = fork();
  int status;

  if (childPID) {
    if (!input->background) {
      waitpid(childPID, &status, 0);
    }
  } else {
    std::shared_ptr<Parser::CommandSeq> commandSeq = input->cs;

    executeCommandSeq(commandSeq);
  }
}

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
