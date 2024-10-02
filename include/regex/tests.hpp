#pragma once
#include <functional>
#include <regex/tree.hpp>
#include <regex/util.hpp>

namespace regex {

class Test {
  std::vector<std::function<bool(char)>> tests;
};

Test compileBracket(Node<uint32_t> bracket_node) {
  if (bracket_node.content != '[')
    throw std::runtime_error("Invalid compileBracket");
  for (unsigned i = 0; i < bracket_node.children.size(); i++) {
  }
}

} // namespace regex