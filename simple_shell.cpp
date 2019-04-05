#include <iostream>
#include <string>
#define MAX_LINE_SIZE 512

int main(int argc, char *argv[]) {
  // bool output = true;
  std::string inputLine = std::string();
  

  while(1) {
    std::cout << "shell:";

    if (std::cin.eof()) {
      break;
    } else {
      std::cin >> inputLine;
    }
    
    std::cout << "Echo:" << inputLine << std::endl;

  }

  return 0;
}