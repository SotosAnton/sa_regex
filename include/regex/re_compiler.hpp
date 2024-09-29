#pragma once
#include "regex/re_util.hpp"

namespace regex {

typedef std::function<bool(char)> reSimpleFun;
typedef std::function<unsigned()> reStateFun;

struct Node {
  std::vector<reSimpleFun> options;

  reStateFun statefunc;

  bool invert = false;

  bool eval(const char &c) {
    bool res = false;
    for (auto f : options) {
      res = res || f(c);
    }
    return invert ^ res;
  }
};

Node compile(const std::string re);

Node evaluateBracket(unsigned &i, const std::string &re);

} // namespace regex