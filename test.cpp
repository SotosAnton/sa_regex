#include <iostream>
#include <regex/re_compiler.hpp>

int main(int argc, const char *argv[]) {

  std::string input = "3";

  std::string re = "[0-9]";

  unsigned count = 0;
  auto bracket = regex::evaluateBracket(count, re);

  std::cout << " Res : " << bracket.eval(input[0]) << '\n';

  // auto result = regex(input, re);
  return 0;
}