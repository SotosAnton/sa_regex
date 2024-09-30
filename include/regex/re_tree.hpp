#pragma once
#include <bits/stdc++.h>
#include <string>
#include <vector>

namespace regex {

struct Node {
  Node(int parent, char content) : parent(parent), content(content){};
  int parent;
  char content;
  std::vector<int> children;
};

struct Tree {
  std::vector<Node> nodes;
  int start_node = 0;

  int back_intex() { return nodes.size() - 1; }

  void push_node(int parent, char content) {
    nodes.push_back(Node(parent, content));
    nodes.at(parent).children.push_back(back_intex());
  }

  void splitNodes(int parent, int child, char content) {

    nodes.push_back(Node(parent, content));

    // nodes.at(parent).children.back() = back_intex();
    std::replace(nodes.at(parent).children.rbegin(),
                 nodes.at(parent).children.rend(), child, back_intex());
    nodes.back().children.push_back(child);
    nodes.at(child).parent = back_intex();

    nodes.at(parent).children;
  }
};

Tree parseToTree(const std::string &i);

Tree &&alignTree(const Tree &tree);

void printReTree(const Tree &tree);

} // namespace regex