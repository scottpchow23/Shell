#include "Tokenizer.hpp"


// struct Tokenizer {
  std::vector<Token> tokenize(std::string input) {
    std::vector<Token> tokens;
    int startOfToken = 0;
    
    for(int i = 0; i < input.length() ; i++) {
      Token newToken = Token(Word, "");
      switch (input[i]) {
        case ' ':
          if (startOfToken != i) {
            newToken = Token(Word, input.substr(startOfToken, i - startOfToken));
            tokens.push_back(newToken);
            startOfToken = i + 1;
          } else {
            startOfToken++;
          }
          break;
        case '<':
          if (startOfToken != i) {
            newToken = Token(Word,input.substr(startOfToken, i - startOfToken));
            tokens.push_back(newToken);
            startOfToken = i + 1;
          } else {
            startOfToken++;
          }
          newToken = Token(LT, std::string(1, input.at(i)));
          tokens.push_back(newToken);
          break;
        case '>':
          if (startOfToken != i) {
            newToken = Token(Word,input.substr(startOfToken, i - startOfToken));
            tokens.push_back(newToken);
            startOfToken = i + 1;
          } else {
            startOfToken++;
          }
          newToken = Token(GT, std::string(1, input.at(i)));
          tokens.push_back(newToken);
          break;
        case '|':
          if (startOfToken != i) {
            newToken = Token(Word,input.substr(startOfToken, i - startOfToken));
            tokens.push_back(newToken);
            startOfToken = i + 1;
          } else {
            startOfToken++;
          }
          newToken = Token(Pipe, std::string(1, input.at(i)));
          tokens.push_back(newToken);
          break;
        case '&':
          if (startOfToken != i) {
            newToken = Token(Word,input.substr(startOfToken, i - startOfToken));
            tokens.push_back(newToken);
            startOfToken = i + 1;
          } else {
            startOfToken++;
          }
          newToken = Token(Amp, std::string(1, input.at(i)));
          tokens.push_back(newToken);
          break;
        default:
          break;
      }
    }

    if (startOfToken < input.length() - 1) {
      Token newToken = Token(Word, input.substr(startOfToken, input.length()));
      tokens.push_back(newToken);
    }

    return tokens;
  }
// };