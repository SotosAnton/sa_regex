#include <iostream>
#include <regex/reTree.hpp>
#include <regex/state_machine.hpp>
#include <regex/state_machine_export.hpp>

int main(int argc, const char *argv[]) {

  std::string input = "3";
  std::string re = "abc[1-5]*x";

  if (argc == 2) {
    re = argv[1];
  } else if (argc == 3) {
    re = argv[1];
    input = argv[2];
  }

  std::cout << re << '\n';
  std::cout << input << '\n';
  regex::Tree tree = regex::parseToTree(re);

  regex::printReTree(tree);

  // auto new_tree = regex::alignTree(tree);

  auto engine = regex::buildStateMachineFromTree(tree);

  std::cout << " Save : "
            << regex::writeMachineDescriptionToFile("engine.txt", engine)
            << '\n';

  std::cout << " Run : " << regex::runStateMachine(engine, input) << '\n';

  return 0;
}