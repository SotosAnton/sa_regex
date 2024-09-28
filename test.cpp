#include <iostream>
#include <regex/regex.hpp>

int main(int argc, const char *argv[]) {

  std::string input = "This is a 5 test for a regex engine";

  std::string re = "";

  auto result = regex(input, re);
  return 0;
}