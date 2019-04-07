#pragma once
#include <tuple>
#include <utility>
#include "Tokenizer.hpp"
namespace Parser {
  struct Word {
    std::string value;
    Word(std::string v){
      this->value = v;
    };
  };

  struct Command {
    std::shared_ptr<Word> in;
    std::shared_ptr<Word> out;
    std::vector<std::shared_ptr<Word>> argv;
    Command(std::shared_ptr<Word>in, std::shared_ptr<Word> out, std::vector<std::shared_ptr<Word>> argv) {
      this->in = in;
      this->out = out;
      this->argv = argv;
    }
  };

  struct CommandSeq {
    std::shared_ptr<Command> left;
    std::shared_ptr<CommandSeq> right;
    CommandSeq(std::shared_ptr<Command> left, std::shared_ptr<CommandSeq> right) {
      this->left = left;
      this->right = right;
    }
  };

  struct Input {
    std::shared_ptr<CommandSeq>cs;
    bool background;
    Input(std::shared_ptr<CommandSeq>cs, bool background) {
      this->cs = cs;
      this->background = background;
    }
  };
}

std::tuple<int, std::shared_ptr<Parser::Word>> parseFile(std::vector<Token> tokens, int cursor) {
  Token token = tokens.at(cursor);
  if (token.type == Word) {
    return std::tuple<int, std::shared_ptr<Parser::Word >>(cursor + 1, new Parser::Word(token.value));
  } else {
    return std::tuple<int, std::shared_ptr<Parser::Word >>(cursor, nullptr);
  }
}

std::tuple<int, std::vector<std::shared_ptr<Parser::Word>>> parseArgs(std::vector<Token> tokens, int cursor) {
  int newCursor = cursor;
  Token token = tokens.at(cursor);
  std::vector<std::shared_ptr<Parser::Word>> words;
  while (token.type == Word) {
    
    words.push_back(std::make_shared<Parser::Word>(token.value));
    ++newCursor;
    token = tokens.at(newCursor);
  }
  return std::tuple<int, std::vector<std::shared_ptr<Parser::Word>>>(newCursor, words);
}

std::tuple<int, std::shared_ptr<Parser::Command>> parseCommand(std::vector<Token> tokens, int cursor, bool ltAllowed, bool gtAllowed) {
  std::shared_ptr<Parser::Word> in;
  std::shared_ptr<Parser::Word> out;
  int newCursor = cursor;
  std::cout << "Starting Args" << std::endl;
  const auto& [cur, words] = parseArgs(tokens, newCursor);
  newCursor = cur;
  std::vector<std::shared_ptr<Parser::Word>> args = words;

  if (tokens[newCursor].type == LT && ltAllowed) {
    std::cout << "Starting infile" << std::endl;
    const auto& [cur2, infile] = parseFile(tokens, newCursor + 1);
    in = infile;
    newCursor = cur2;
  }

  if (tokens[newCursor].type == GT && gtAllowed) {
    std::cout << "Starting outfile" << std::endl;
    const auto& [cur2, outfile] = parseFile(tokens, newCursor + 1);
    out = outfile;
    newCursor = cur2;
  }
  std::cout << "[Command] args: ";
  for (std::shared_ptr<Parser::Word> word : args) {
    std::cout << word->value << " ";
  }
  if (in != nullptr) {
    std::cout << "| in: " << in->value << " ";
  }
  if (out != nullptr) {
    std::cout << "| out: " <<  out->value;
  }
  std::cout << std::endl;
  return std::tuple<int, std::shared_ptr<Parser::Command>>(newCursor, std::make_shared<Parser::Command>(in, out, args));
}

std::tuple<int, std::shared_ptr<Parser::CommandSeq>> parseCommandSeq(std::vector<Token> tokens, int cursor) {
  int newCursor = cursor;
  std::cout << "Starting first Command" << std::endl;
  const auto& [cur, cmd] = parseCommand(tokens, newCursor, true, true);
  newCursor = cur;
  Token token = tokens.at(newCursor);
  std::shared_ptr<Parser::CommandSeq> headCS = std::make_shared<Parser::CommandSeq>(cmd, nullptr);
  std::shared_ptr<Parser::CommandSeq> curCS = headCS;
  while (token.type == Pipe) {
    // check that the previous command didn't redirect output
    if (curCS->left->out != nullptr) {
      // syntax error because command before pipe redirected output
      break;
    }

    newCursor++;
    std::cout << "Starting another Command" << std::endl;
    const auto& [cur2, cmd2] = parseCommand(tokens, newCursor, false, true);
    // error check that cmd2 resulted in full command
    if (cmd2->argv.empty()) {
      break;
    }
    newCursor = cur2;

    curCS->right = std::make_shared<Parser::CommandSeq>(cmd2, nullptr);
    curCS = curCS->right;
    token = tokens.at(newCursor);
  }

  return std::tuple<int, std::shared_ptr<Parser::CommandSeq>>(newCursor, headCS);
}

std::tuple<int, std::shared_ptr<Parser::Input>> parseInput(std::vector<Token> tokens) {
  std::cout << "Starting CommandSeq" << std::endl;
  const auto& [cur, cs] = parseCommandSeq(tokens, 0);
  int newCursor = cur;
  bool background = false;
  if (tokens[cur].type == Amp) {
    background = true;
    newCursor++;
  }

  return std::tuple<int, std::shared_ptr<Parser::Input>>(newCursor, std::make_shared<Parser::Input>(cs, background));
}

std::tuple<bool, std::shared_ptr<Parser::Input>> parse(std::vector<Token> tokens) {
  std::cout << "Starting Input" << std::endl;
  const auto& [cur, input] = parseInput(tokens);
  bool parsed = (tokens[cur].type == EOI);
  return std::tuple<bool, std::shared_ptr<Parser::Input>>(parsed, input);
}
