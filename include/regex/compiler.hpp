#pragma once
#include "functional"
#include "regex/util.hpp"

namespace regex {

typedef std::function<unsigned(unsigned &i, char c)> NodeFunction;

struct State {
  NodeFunction func;
};

} // namespace regex
