#include <iostream>
#include <regex/re_compiler.hpp>
#include <regex/re_tree.hpp>

int main(int argc, const char *argv[]) {

  std::string input = "3";

  std::string re = "[0-9[aaa]][a-c]*";
  re = "^[aaa[33]]*[0-9]";
  re = "((a)(-^-)[-0-]-[[][]])";

  // unsigned count = 0;
  // auto bracket = regex::evaluateBracket(count, re);
  // std::cout << " Res : " << bracket.eval(input[0]) << '\n';
  // auto result = regex(input, re);
  std::cout << re << '\n';
  regex::Tree tree = regex::parseToTree(re);

  regex::printReTree(tree);

  return 0;
}