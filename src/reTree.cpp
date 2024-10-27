#include <iostream>
#include <queue>
#include <regex/reTree.hpp>
#include <stack>

namespace regex {

ReTree parseToTree(const std::string &re_str) {

  std::stack<unsigned> parent_node_stack;
  parent_node_stack.push(0);

  ReTree tree;

  tree.nodes.emplace_back(-1, OpCode::ROOT);

  int last_node;
  unsigned i = 0;
  while (i < re_str.size()) {
    char c = re_str[i];
    switch (c) {
    case '^':
      if (i == 0) {
        tree.push_node(parent_node_stack.top(), OpCode::AT_START);
        parent_node_stack.push(tree.back_intex());
      } else {
        if (re_str[i - 1] == '[')
          if (tree.nodes.at(tree.back_intex()).content != OpCode::BRACKET)
            throw std::runtime_error("Bracket operator fail");
        tree.nodes.at(tree.back_intex()).content = OpCode::INV_BRACKET;
      }
      break;
    case '[':
      tree.push_node(parent_node_stack.top(), OpCode::BRACKET);
      parent_node_stack.push(tree.back_intex());
      break;
    case ']':
      if (tree.nodes.empty() ||
          (tree.nodes.at(parent_node_stack.top()).content != OpCode::BRACKET &&
           tree.nodes.at(parent_node_stack.top()).content !=
               OpCode::INV_BRACKET)) {
        tree.nodes.emplace_back(parent_node_stack.top(), ']');
      } else {
        parent_node_stack.pop();
      }
      break;
    case '(':
      tree.push_node(parent_node_stack.top(), OpCode::SUBEXPRESSION);
      parent_node_stack.push(tree.back_intex());
      break;
    case ')':
      if (tree.nodes.empty() ||
          tree.nodes.at(parent_node_stack.top()).content !=
              OpCode::SUBEXPRESSION) {
        throw std::runtime_error(
            "Paranthesis Missmatch :" +
            tree.nodes.at(parent_node_stack.top()).content);
      } else {
        parent_node_stack.pop();
      }
      break;
    case '-':
      if ((tree.nodes.at(parent_node_stack.top()).content ==
               OpCode::INV_BRACKET ||
           tree.nodes.at(parent_node_stack.top()).content == OpCode::BRACKET) &&
          (i + 1 < re_str.size() - 1) && re_str[i + 1] != ']' && (i - 1 > 0) &&
          re_str[i - 1] != '[') {
        last_node = tree.nodes.at(parent_node_stack.top()).children.back();
        tree.splitNodes(parent_node_stack.top(), last_node, OpCode::RANGE);
        tree.push_node(tree.back_intex(), re_str[++i]);
      } else {
        tree.push_node(parent_node_stack.top(), '-');
      }
      break;
    case '.':
      tree.push_node(parent_node_stack.top(), OpCode::WILDCARD);
      break;
    case '$':
      if (i == re_str.size() - 1) {
        tree.push_node(0, OpCode::AT_END);
      } else {
        tree.push_node(parent_node_stack.top(), '$');
      }
      break;
    case '*':
      last_node = tree.nodes.at(parent_node_stack.top()).children.back();
      tree.splitNodes(parent_node_stack.top(), last_node, OpCode::KLEENE_STAR);
      break;
    case '+':
      last_node = tree.nodes.at(parent_node_stack.top()).children.back();
      tree.splitNodes(parent_node_stack.top(), last_node, OpCode::REPETITION);
      break;
    case '?':
      last_node = tree.nodes.at(parent_node_stack.top()).children.back();
      tree.splitNodes(parent_node_stack.top(), last_node, OpCode::OPTIONAL);
      break;
    default:
      tree.push_node(parent_node_stack.top(), c);
    };
    i++;
  }

  while (!parent_node_stack.empty()) {
    auto node_id = parent_node_stack.top();
    parent_node_stack.pop();
    if (tree.nodes.at(node_id).content != OpCode::ROOT) {
      DEBUG_STDOUT("Unresolved content : "
                   << static_cast<OpCode>(tree.nodes.at(node_id).content)
                   << '\n')
    }
  }
  // return std::move(tree);
  return tree;
}

ReTree alignTree(const ReTree &tree) {
  ReTree new_tree;
  std::queue<unsigned> parent_node_stack;
  std::queue<unsigned> new_parent_node_stack;

  parent_node_stack.push(tree.start_node);
  new_parent_node_stack.push(0);

  const ReNode &start_node = tree.nodes.at(tree.start_node);

  new_tree.nodes.emplace_back(new_parent_node_stack.front(),
                              start_node.content);

  while (!parent_node_stack.empty()) {

    int current_node_id = parent_node_stack.front();
    parent_node_stack.pop();
    const ReNode &current_node = tree.nodes.at(current_node_id);

    int new_tree_current_node_id = new_parent_node_stack.front();
    new_parent_node_stack.pop();

    for (unsigned j = 0; j < current_node.children.size(); j++) {
      int child_id = current_node.children.at(j);

      new_tree.nodes.emplace_back(new_tree_current_node_id,
                                  tree.nodes.at(child_id).content);

      ReNode &new_current_node = new_tree.nodes.at(new_tree_current_node_id);
      new_current_node.children.push_back(new_tree.back_intex());

      new_parent_node_stack.push(new_tree.back_intex());

      parent_node_stack.push(child_id);
    }
  }

  return new_tree;
}

void printReTree(const ReTree &tree) {
  std::queue<int> parent_node_stack;
  std::queue<int> level_stack;

  parent_node_stack.push(tree.start_node);
  level_stack.push(0);

  const ReNode &start_node = tree.nodes.at(tree.start_node);

  std::cout << static_cast<OpCode>(start_node.content) << '\n';

  while (!parent_node_stack.empty()) {

    int current_node_id = parent_node_stack.front();
    int current_level = level_stack.front();

    parent_node_stack.pop();
    level_stack.pop();

    const ReNode &current_node = tree.nodes.at(current_node_id);

    for (unsigned j = 0; j < current_node.children.size(); j++) {
      int child_id = current_node.children.at(j);

      std::cout << '(' << tree.nodes.at(child_id).parent << ',' << child_id
                << ')' << static_cast<OpCode>(tree.nodes.at(child_id).content)
                << " ";

      for (unsigned k = 1; k < tree.nodes.at(child_id).children.size(); k++)
        std::cout << "       ";

      parent_node_stack.push(child_id);
      level_stack.push(current_level + 1);
    }

    if (current_level != level_stack.front())
      std::cout << '\n';
  }
}

} // namespace regex