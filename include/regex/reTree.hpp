#pragma once
#include <bits/stdc++.h>
#include <regex/opcode.hpp>
#include <regex/tree.hpp>

namespace regex {

typedef Tree<uint32_t> ReTree;
typedef Node<uint32_t> ReNode;

// uint32_t OpCode::operator uint32_t(opcode)
ReTree parseToTree(const std::string &i);

ReTree alignTree(const ReTree &tree);

void printReTree(const ReTree &tree);

OpCode parseBackSlash(size_t &index, const std::string &i);

struct CountedQuantifier {
  int min;
  int max;
  bool range_flag;
  CountedQuantifier(int min, int max, bool r)
      : min(min), max(max), range_flag(r){};
};

CountedQuantifier parseCount(const std::string &re_str, size_t &index);

void buildCountNodes(const CountedQuantifier counter, ReTree &tree,
                     std::stack<unsigned> &parent_node_stack);

} // namespace regex