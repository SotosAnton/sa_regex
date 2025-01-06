
#include <regex/sa_regex.hpp>
namespace sa_ {
Regex::Regex(const std::string &re) {
  regex::ReTree tree = regex::parseToTree(re);
  engine = regex::buildStateMachineFromTree(tree);
  executor = regex::StateMachineExecutor(&engine);
}

bool Regex::run(const std::string input,
                std::vector<std::pair<size_t, size_t>> *matches) {
  return executor.run(input, matches);
}

} // namespace sa_