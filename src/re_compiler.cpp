#include <iostream>
#include <regex/re_compiler.hpp>
#include <stack>
#include <stdexcept>

namespace regex {

Node compile(const std::string re) {
  std::vector<Node> nodes;

  unsigned i = 0;
  while (i < re.size()) {
    char c = re[i];
    switch (c) {
    case '[':
      nodes.push_back(evaluateBracket(i, re));
      nodes.back().options.push_back(
          std::bind(&Node::eval, nodes.back(), std::placeholders::_1));
      break;
    case ']':
      /* code */
      break;
    case '.':
      /* code */
      break;
    case '^':
      /* code */
      break;
    case '&':
      /* code */
      break;
    case '*':
      /* code */
      break;

    default:
      break;
    }
  }
}

Node evaluateBracket(unsigned &i, const std::string &re) {
  Node bracket;
  std::stack<char> stack;
  bool bracket_closed = false;

  while (!bracket_closed) {
    if (i >= re.size())
      throw std::runtime_error("Invalid bracket expression");
    switch (re[i]) {
    case '^':
      if (re[i - 1] == '[')
        bracket.invert = true;
      else
        stack.push(re[i]);
      break;
    case '[':
      if (i != 0)
        stack.push(re[i]);
      break;
    case ']':
      bracket_closed = true;
      break;
    case '-':
      if (stack.empty()) {
        bracket.options.push_back(
            std::bind(isEqual, std::placeholders::_1, '-'));
        std::cout << "isEqual " << '-' << "\n";

      } else {
        if (i + 2 > re.size() - 1 || re[i + 1] == ']' || re[i + 1] == '-')
          throw std::runtime_error("Invalid bracket expression");
        bracket.options.push_back(
            std::bind(isInRange, std::placeholders::_1, re[i - 1], re[i + 1]));
        std::cout << "isInRange " << re[i - 1] << '-' << re[i + 1] << "\n";
        stack.pop();
        i++;
      }
      break;
    default:
      stack.push(re[i]);
    }
    i++;
  }

  while (!stack.empty()) {
    bracket.options.push_back(
        std::bind(isEqual, std::placeholders::_1, stack.top()));
    std::cout << "isEqual " << stack.top() << "\n";

    stack.pop();
  }
  return bracket;
}
} // namespace regex