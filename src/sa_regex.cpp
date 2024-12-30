
#include <regex/sa_regex.hpp>
namespace sa_ {
Regex::Regex(const std::string &re) {
  regex::ReTree tree = regex::parseToTree(re);
  engine = regex::buildStateMachineFromTree(tree);
}

bool Regex::operator()(const std::string input) const {
  return regex::runStateMachineSmart(engine, input);
}
} // namespace sa_