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

  size_t last_node;
  size_t i = 0;
  while (i < re_str.size()) {

    char c = re_str[i];

    switch (c) {
    case '\\':
      tree.push_node(parent_node_stack.top(), parseBackSlash(i, re_str));
      break;
    case '^':
      if (i == 0) {
        tree.push_node(parent_node_stack.top(), OpCode::AT_START);
        parent_node_stack.push(tree.size() - 1);
      } else {
        if (re_str[i - 1] == '[')
          if (tree.nodes.at(tree.size() - 1).content != OpCode::BRACKET)
            throw std::runtime_error("Bracket operator fail");
        tree.nodes.at(tree.size() - 1).content = OpCode::INV_BRACKET;
      }
      break;
    case '[':
      tree.push_node(parent_node_stack.top(), OpCode::BRACKET);
      parent_node_stack.push(tree.size() - 1);
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
      parent_node_stack.push(tree.size() - 1);
      break;
    case ')':
      if (tree.nodes.empty()) {
        throw std::runtime_error(
            "Paranthesis Missmatch :" +
            tree.nodes.at(parent_node_stack.top()).content);
      } else if (tree.nodes.at(parent_node_stack.top()).content ==
                 OpCode::SUBEXPRESSION) {
        parent_node_stack.pop();

      } else if (tree.nodes.at(parent_node_stack.top()).content ==
                 OpCode::UNION_SUBEXPRESION) {
        parent_node_stack.pop();
        parent_node_stack.pop();

      } else {
        throw std::runtime_error(
            "Paranthesis Missmatch :" +
            tree.nodes.at(parent_node_stack.top()).content);
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
        tree.push_node(tree.size() - 1, re_str[++i]);
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
    case '|':

      // Special case to avoid nesting union nodes
      if (tree.getNodeContent(parent_node_stack.top()) ==
          OpCode::UNION_SUBEXPRESION) {
        auto temp_top = parent_node_stack.top();
        parent_node_stack.pop();
        if (!parent_node_stack.empty() &&
            tree.getNodeContent(parent_node_stack.top()) == OpCode::UNION) {
          tree.push_node(parent_node_stack.top(), OpCode::UNION_SUBEXPRESION);
          parent_node_stack.push(temp_top);
          parent_node_stack.push(tree.size() - 1);
          break;
        }
        parent_node_stack.push(temp_top);
      }

      tree.rebaseNode(parent_node_stack.top(), OpCode::UNION_SUBEXPRESION);
      tree.splitNodes(parent_node_stack.top(), tree.size() - 1, OpCode::UNION);
      parent_node_stack.push(tree.size() - 1);
      tree.push_node(parent_node_stack.top(), OpCode::UNION_SUBEXPRESION);
      parent_node_stack.push(tree.size() - 1);

      break;
    case '+':
      last_node = tree.nodes.at(parent_node_stack.top()).children.back();
      tree.splitNodes(parent_node_stack.top(), last_node, OpCode::REPETITION);
      break;
    case '?':
      if (tree.nodes.at(parent_node_stack.top()).children.empty())
        throw std::runtime_error("Regex parse error. Invalid: '?' \n");

      last_node = tree.nodes.at(parent_node_stack.top()).children.back();

      if (tree.nodes.at(last_node).content == OpCode::REPETITION) {
        tree.nodes.at(last_node).content = OpCode::LAZY_REPETITION;
        break;
      } else if (tree.nodes.at(last_node).content == OpCode::OPTIONAL) {
        tree.nodes.at(last_node).content = OpCode::LAZY_OPTIONAL;
        break;
      } else if (tree.nodes.at(last_node).content == OpCode::COUNT) {
        tree.nodes.at(last_node).content = OpCode::LAZY_COUNT;
        break;
      } else if (tree.nodes.at(last_node).content == OpCode::KLEENE_STAR) {
        tree.nodes.at(last_node).content = OpCode::LAZY_KLEENE_STAR;
        break;
      }

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
      new_current_node.children.push_back(new_tree.size() - 1);

      new_parent_node_stack.push(new_tree.size() - 1);

      parent_node_stack.push(child_id);
    }
  }

  return new_tree;
}

void printReTree(const ReTree &tree) {
  std::queue<int> parent_node_stack;
  std::queue<int> level_stack;

#ifdef DEBUG
  std::vector<bool> visited(tree.size(), false);
#endif

  parent_node_stack.push(tree.start_node);
  level_stack.push(0);

  const ReNode &start_node = tree.nodes.at(tree.start_node);

  std::cout << static_cast<OpCode>(start_node.content) << '\n';

  while (!parent_node_stack.empty()) {

    int current_node_id = parent_node_stack.front();

#ifdef DEBUG

    if (visited[current_node_id])
      throw std::runtime_error(
          "Tree Parse error: Tree contains cyrcle. Index :" +
          std::to_string(current_node_id));
#endif

    visited[current_node_id] = true;

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

OpCode parseBackSlash(size_t &index, const std::string &i) {
  if (index + 1 >= i.size())
    return static_cast<OpCode>('\\');

  const char next_char = i[index + 1];
  index++;

  DEBUG_STDOUT(" backslash : " << next_char << '\n');

  switch (next_char) {
    // Regex specific codes
  case 'S':
    return OpCode::NON_WHITESPACE;
  case 'w':
    return OpCode::WORD_CHAR;
  case 'W':
    return OpCode::NON_WORD_CHAR;
  case 'd':
    return OpCode::DIGIT;
  case 'D':
    return OpCode::NON_DIGIT;
    // general special chars
  case 'n':
    return static_cast<OpCode>('\n');
  case 't':
    return static_cast<OpCode>('\t');
  case 'r':
    return static_cast<OpCode>('\r');
  case 'b':
    return static_cast<OpCode>('\b');
    // Avoid regex specific codes
  case '*':
    return static_cast<OpCode>('*');
  case '(':
    return static_cast<OpCode>('(');
  case '[':
    return static_cast<OpCode>('[');
  case '{':
    return static_cast<OpCode>('}');
  case ')':
    return static_cast<OpCode>(')');
  case ']':
    return static_cast<OpCode>(']');
  case '}':
    return static_cast<OpCode>('}');
  case '.':
    return static_cast<OpCode>('.');
  case '|':
    return static_cast<OpCode>('|');
  case '\\':
    return static_cast<OpCode>('\\');
  case 'x': // Hex
    throw std::runtime_error("Backslash Hex not implemented\n");
    return static_cast<OpCode>(next_char);
  case 'u': // Unicode
    throw std::runtime_error("Backslash Unicode not implemented\n");
    return static_cast<OpCode>(next_char);

  default:
    index--;
    return static_cast<OpCode>('\\');
    break;
  }
}

} // namespace regex