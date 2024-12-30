#pragma once
#include <iostream>
#include <regex/builder.hpp>
#include <regex/reTree.hpp>
#include <regex/state_machine.hpp>
#include <regex/state_machine_export.hpp>

namespace sa_ {

class Regex {
public:
  Regex(const std::string &re);
  Regex(){};
  bool operator()(const std::string input) const;

private:
  regex::StateMachine engine;
};

} // namespace sa_