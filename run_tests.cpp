#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <regex/builder.hpp>

#include <regex/reTree.hpp>
#include <regex/state_machine.hpp>

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

    regex::Tree tree = regex::parseToTree(re);

    auto engine = regex::buildStateMachineFromTree(tree);

    for (auto &match_case : match) {
      total++;
      if (!regex::runStateMachine(engine, match_case))
        std::cout << " Re:" << re << " false Negative : " << match_case << '\n';
      else
        pass++;
    }

    for (auto &skip_case : skip) {
      total++;
      if (regex::runStateMachine(engine, skip_case))
        std::cout << " Re:" << re << " False Positive : " << skip_case << '\n';
      else
        pass++;
    }
  }

  std::cout << "Test Result: " << pass << '/' << total << '\n';
}
