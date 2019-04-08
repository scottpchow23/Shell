#pragma once
#include <string>
#include <vector>
#include <iostream>
#define MAX_TOKEN_SIZE 32

enum TokenType {
  Word, GT, LT, Pipe, Amp, EOI
};

struct Token {
  TokenType type;
  std::string value;
  Token(TokenType type,std::string value) { 
    this->type = type;
    this->value = value;
  };
};

std::vector<Token> tokenize(std::string input);