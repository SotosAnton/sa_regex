#include <iostream>
#include <queue>
#include <regex/re_tree.hpp>
#include <stack>

namespace regex {

Tree parseToTree(const std::string &re_str) {

  std::stack<unsigned> parent_node_stack;
  parent_node_stack.push(0);

  Tree tree;
  tree.nodes.push_back(Node(-1, '@'));

  int last_node;
  unsigned i = 0;
  while (i < re_str.size()) {
    char c = re_str[i];
    switch (c) {
    case '^':
      if (i == 0) {
        tree.push_node(parent_node_stack.top(), '^');
        parent_node_stack.push(tree.back_intex());
      } else {
        tree.push_node(parent_node_stack.top(), '^');
      }
      break;
    case '[':
      tree.push_node(parent_node_stack.top(), '[');
      parent_node_stack.push(tree.back_intex());
      break;
    case ']':
      if (tree.nodes.empty() ||
          tree.nodes.at(parent_node_stack.top()).content != '[') {
        tree.nodes.push_back(Node(parent_node_stack.top(), '['));
      } else {
        parent_node_stack.pop();
      }
      break;
    case '(':
      tree.push_node(parent_node_stack.top(), '(');
      parent_node_stack.push(tree.back_intex());
      break;
    case ')':
      if (tree.nodes.empty() ||
          tree.nodes.at(parent_node_stack.top()).content != '(') {
        throw std::runtime_error(
            "Paranthesis Missmatch :" +
            tree.nodes.at(parent_node_stack.top()).content);
      } else {
        parent_node_stack.pop();
      }
      break;
    case '-':
      if (tree.nodes.at(parent_node_stack.top()).content == '[' &&
          (i + 1 < re_str.size() - 1) && re_str[i + 1] != ']' && (i - 1 > 0) &&
          re_str[i - 1] != '[') {
        last_node = tree.nodes.at(parent_node_stack.top()).children.back();
        tree.splitNodes(parent_node_stack.top(), last_node, '-');
        tree.push_node(tree.back_intex(), re_str[++i]);
      } else {
        tree.push_node(parent_node_stack.top(), '-');
      }
      break;
    case '.':
      tree.push_node(parent_node_stack.top(), '.');
    case '$':
      tree.push_node(parent_node_stack.top(), '$');
      break;
    case '*':
      last_node = tree.nodes.at(parent_node_stack.top()).children.back();
      tree.splitNodes(parent_node_stack.top(), last_node, '*');
      break;
    default:
      tree.push_node(parent_node_stack.top(), c);
    };
    i++;
  }

  while (!parent_node_stack.empty()) {
    auto node_id = parent_node_stack.top();
    parent_node_stack.pop();
    std::cout << "Unresolved content : " << tree.nodes.at(node_id).content
              << '\n';
  }
  // return std::move(tree);
  return tree;
}

Tree &&alignTree(const Tree &tree) {
  Tree new_tree;
  std::stack<unsigned> parent_node_stack;
  std::stack<unsigned> new_parent_node_stack;

  parent_node_stack.push(tree.start_node);
  new_parent_node_stack.push(0);

  const Node &start_node = tree.nodes.at(tree.start_node);

  new_tree.nodes.push_back(
      Node(new_parent_node_stack.top(), start_node.content));

  while (!parent_node_stack.empty()) {

    int current_node_id = parent_node_stack.top();
    parent_node_stack.pop();
    const Node &current_node = tree.nodes.at(current_node_id);

    int new_tree_current_node_id = parent_node_stack.top();
    new_parent_node_stack.pop();

    const Node &new_parent_node = tree.nodes.at(current_node_id);

    for (unsigned j = 0; j < current_node.children.size(); j++) {
      int child_id = current_node.children.at(j);

      new_tree.nodes.push_back(
          Node(new_tree_current_node_id, tree.nodes.at(child_id).content));
      new_parent_node_stack.push(new_tree.back_intex());

      parent_node_stack.push(child_id);
    }
  }
}

void printReTree(const Tree &tree) {
  std::queue<int> parent_node_stack;
  std::queue<int> level_stack;

  parent_node_stack.push(tree.start_node);
  level_stack.push(0);

  const Node &start_node = tree.nodes.at(tree.start_node);

  std::cout << start_node.content << '\n';

  while (!parent_node_stack.empty()) {

    int current_node_id = parent_node_stack.front();
    int current_level = level_stack.front();

    parent_node_stack.pop();
    level_stack.pop();

    const Node &current_node = tree.nodes.at(current_node_id);

    for (unsigned j = 0; j < current_node.children.size(); j++) {
      int child_id = current_node.children.at(j);

      std::cout << '(' << tree.nodes.at(child_id).parent << ',' << child_id
                << ')' << tree.nodes.at(child_id).content << " ";

      for (unsigned k = 1; k < tree.nodes.at(child_id).children.size(); k++)
        std::cout << "    ";

      parent_node_stack.push(child_id);
      level_stack.push(current_level + 1);
    }

    if (current_level != level_stack.front())
      std::cout << '\n';
  }
}

} // namespace regex