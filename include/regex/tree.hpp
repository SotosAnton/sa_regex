#pragma once
#include <bits/stdc++.h>
#include <regex/util.hpp>
#include <string>
#include <vector>

namespace regex {

template <typename T> struct Node {
  Node(int parent, T content) : parent(parent), content(content){};

  int parent;
  T content;
  std::vector<int> children;
};

template <typename T> struct Tree {
  std::vector<Node<T>> nodes;
  int start_node = 0;

  int back_intex() { return nodes.size() - 1; }

  void push_node(int parent, T content) {
    nodes.emplace_back(parent, content);
    nodes.at(parent).children.push_back(back_intex());
  }

  void splitNodes(int parent, int child, T content) {

    nodes.emplace_back(parent, content);

    // nodes.at(parent).children.back() = back_intex();
    std::replace(nodes.at(parent).children.rbegin(),
                 nodes.at(parent).children.rend(), child, back_intex());
    nodes.back().children.push_back(child);
    nodes.at(child).parent = back_intex();

    nodes.at(parent).children;
  }
  const Node<T> &getNode(const unsigned i) const { return nodes.at(i); }
  Node<T> &getNode(const unsigned i) { return nodes.at(i); }
};

} // namespace regex