#pragma once
#include <bits/stdc++.h>
#include <regex/util.hpp>
#include <string>
#include <vector>

namespace regex {

template <typename T> struct Node {
  Node(size_t parent, T content) : parent(parent), content(content){};

  size_t parent;
  T content;
  std::vector<size_t> children;
};

template <typename T> struct Tree {
  std::vector<Node<T>> nodes;
  int start_node = 0;

  // size_t back_intex() { return nodes.size() - 1; }
  size_t size() const { return nodes.size(); }

  void push_node(const int parent, const T content) {
    nodes.emplace_back(parent, content);
    nodes.at(parent).children.push_back(nodes.size() - 1);
  }

  void splitNodes(const size_t parent, const size_t child, const T &content) {

    nodes.emplace_back(parent, content);

    std::replace(nodes.at(parent).children.rbegin(),
                 nodes.at(parent).children.rend(), child, nodes.size() - 1);
    nodes.back().children.push_back(child);
    nodes.at(child).parent = nodes.size() - 1;

    nodes.at(parent).children;
  };

  void rebaseNode(const size_t node_id, const T &content) {

    nodes.emplace_back(node_id, content);
    size_t new_node_id = nodes.size() - 1;

    auto &root_children = nodes.at(node_id).children;
    auto &new_children = nodes.at(new_node_id).children;

    new_children = root_children;

    for (auto idx : new_children)
      nodes.at(idx).parent = new_node_id;

    nodes.at(new_node_id).parent = node_id;

    root_children = {new_node_id};
  }

  const Node<T> &getNode(const unsigned i) const { return nodes.at(i); }
  Node<T> &getNode(const unsigned i) { return nodes.at(i); }

  T &getNodeContent(const unsigned i) { return nodes.at(i).content; }
  const T &getNodeContent(const unsigned i) const {
    return nodes.at(i).content;
  }
};

} // namespace regex