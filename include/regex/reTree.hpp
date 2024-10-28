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

} // namespace regex