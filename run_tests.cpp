#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <regex/builder.hpp>

#include <regex/sa_regex.hpp>

using json = nlohmann::json;

int main(int argc, const char *argv[]) {
  std::string test_case_file = "../tests/test_set_1.json";
  std::string report_file = "../tests/report_test_set_1.json";
  std::ifstream fh(test_case_file);
  std::ofstream out_fh(report_file);

  auto data = json::parse(fh);

  int pass = 0;
  int total = 0;

  bool verbose = false;

  for (auto &test_case : data) {

    std::string re = test_case.at("re").get<std::string>();
    std::vector<std::string> match =
        test_case.at("match").get<std::vector<std::string>>();
    std::vector<std::string> skip =
        test_case.at("skip").get<std::vector<std::string>>();

    std::cout << "Testing re : " << re << "\n";

    std::unique_ptr<sa_::Regex> engine;

    try {
      engine = std::make_unique<sa_::Regex>(re);
    } catch (std::exception &e) {
      std::cerr << "\nError while building regex engine.\n re: " << re
                << " Error : " << e.what() << '\n';
      break;
    }

    std::string *case_save = nullptr;

    try {
      for (auto &match_case : match) {

        if (verbose)
          std::cout << "Match case : " << match_case << "\n";

        case_save = &match_case;
        total++;
        if (!engine->match(match_case))
          out_fh << " Re:" << re << " false Negative : " << match_case << '\n';
        else
          pass++;
      }

      for (auto &skip_case : skip) {
        if (verbose)
          std::cout << "Skip case : " << skip_case << "\n";

        case_save = &skip_case;
        total++;
        if (engine->match(skip_case))
          out_fh << " Re:" << re << " False Positive : " << skip_case << '\n';
        else
          pass++;
      }
    } catch (std::exception &e) {
      std::cerr << "\nError while testing.\n re: " << re
                << "\ninput: " << (case_save ? *case_save : " ** fail **  ")
                << '\n'
                << " Error : " << e.what() << '\n';

      try {
        std::cout << " Save : " << engine->saveTofile("engine.txt") << '\n';
      } catch (std::exception &e) {
        std::cerr << "Failed to export state machine: " << e.what() << '\n';
      }

      break;
    }
  }

  out_fh << "Test Result: " << pass << '/' << total << '\n';
  std::cout << "Test Result: " << pass << '/' << total << '\n';

  out_fh.close();
}
