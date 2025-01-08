
#include <regex/sa_regex.hpp>
namespace sa_ {
Regex::Regex(const std::string &re) {
  regex::ReTree tree = regex::parseToTree(re);
  engine = regex::buildStateMachineFromTree(tree);
  executor = regex::StateMachineExecutor(&engine);
}

bool Regex::match(const std::string input) { return executor.match(input); }
bool Regex::search(const std::string input,
                   std::vector<std::pair<size_t, size_t>> *matches) {
  return executor.search(input, matches);
}
bool Regex::scan(const std::string input,
                 std::vector<std::pair<size_t, size_t>> *matches) {
  return executor.scan(input, matches);
}

} // namespace sa_