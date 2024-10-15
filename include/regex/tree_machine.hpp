#pragma once
#include "functional"
#include "regex/reTree.hpp"
#include "regex/util.hpp"

namespace regex {

struct StateLeaf {
  std::vector<unsigned> children;
  unsigned parent;
};

struct TreeMachine {
  std::vector<ReNode> tree;
};

TreeMachine buildTreeMachineFromTree(const ReTree &tree);

} // namespace regex
