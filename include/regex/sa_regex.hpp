#pragma once
#include <iostream>
#include <regex/builder.hpp>
#include <regex/reTree.hpp>
#include <regex/state_machine.hpp>
#include <regex/state_machine_executor.hpp>
#include <regex/state_machine_export.hpp>

namespace sa_ {

class Regex {
public:
  Regex(const std::string &re);
  Regex(){};
  bool operator()(const std::string input) { return match(input); };
  bool match(const std::string input);
  bool search(const std::string input,
              std::vector<std::pair<size_t, size_t>> *matches = nullptr);
  bool scan(const std::string input,
            std::vector<std::pair<size_t, size_t>> *matches = nullptr);
  size_t size() const { return engine.size(); }

  bool saveTofile(const std::string &filename) {
    return writeMachineDescriptionToFile(filename, engine);
  };

private:
  regex::StateMachine engine;
  regex::StateMachineExecutor executor;
};

} // namespace sa_