#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <regex/builder.hpp>

#include <regex/reTree.hpp>
#include <regex/state_machine.hpp>
#include <regex/state_machine_export.hpp>

using json = nlohmann::json;

int main(int argc, const char *argv[]) {
  std::string test_case_file = "../tests/test_set_1.json";
  std::ifstream fh(test_case_file);

  auto data = json::parse(fh);

  int pass = 0;
  int total = 0;

  for (auto &test_case : data) {
    std::string re = test_case.at("re").get<std::string>();
    std::vector<std::string> match =
        test_case.at("match").get<std::vector<std::string>>();
    std::vector<std::string> skip =
        test_case.at("skip").get<std::vector<std::string>>();

    regex::StateMachine engine;
    regex::ReTree tree;

    try {
      tree = regex::parseToTree(re);
    } catch (std::exception &e) {
      std::cerr << "\nError while parsing.\n re: " << re
                << " Error : " << e.what() << '\n';
      break;
    }

    try {
      engine = regex::buildStateMachineFromTree(tree);
    } catch (std::exception &e) {
      std::cerr << "\nError while buidling state machine.\n re: " << re
                << " Error : " << e.what() << '\n';
      break;
    }
    std::string *case_save = nullptr;

    try {
      for (auto &match_case : match) {
        case_save = &match_case;
        total++;
        if (!regex::runStateMachine(engine, match_case))
          std::cout << " Re:" << re << " false Negative : " << match_case
                    << '\n';
        else
          pass++;
      }

      for (auto &skip_case : skip) {
        case_save = &skip_case;
        total++;
        if (regex::runStateMachine(engine, skip_case))
          std::cout << " Re:" << re << " False Positive : " << skip_case
                    << '\n';
        else
          pass++;
      }
    } catch (std::exception &e) {
      std::cerr << "\nError while testing.\n re: " << re
                << "\ninput: " << (case_save ? *case_save : " ** fail **  ")
                << '\n'
                << " Error : " << e.what() << '\n';

      try {
        std::cout << " Save : "
                  << regex::writeMachineDescriptionToFile("engine.txt", engine)
                  << '\n';
      } catch (std::exception &e) {
        std::cerr << "Failed to export state machine: " << e.what() << '\n';
      }

      break;
    }
  }

  std::cout << "Test Result: " << pass << '/' << total << '\n';
}
