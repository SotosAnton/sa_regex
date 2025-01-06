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
  bool operator()(const std::string input,
                  std::vector<std::pair<size_t, size_t>> *matches) {
    return run(input, matches);
  };
  bool run(const std::string input,
           std::vector<std::pair<size_t, size_t>> *matches);
  size_t size() const { return engine.size(); }

private:
  regex::StateMachine engine;
  regex::StateMachineExecutor executor;
};

} // namespace sa_