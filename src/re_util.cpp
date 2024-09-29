#include "regex/re_util.hpp"

namespace regex {

// Metacharacter .
// Match any character
bool anySingleChar(size_t &c, const std::string &input) {
  c++;
  return true;
}

// Metacharacter [^ ]
// Match any character not in set
bool invertFunction(size_t &c, const std::string &input,
                    const reFunction &func) {
  return !func(c, input);
}

// Metacharacter ^
// Matches the starting position within the string, if it is the first character
// of the regular expression.
bool startPoint(size_t &c, const std::string &input, const reFunction &func) {
  if (c == 0)
    return func(c, input);
  else
    return false;
}

// Metacharacter $
// Matches the ending position of the string, if it is the last character of the
// regular expression.
bool endPoint(size_t &c, const std::string &input, const reFunction &func) {
  bool res = func(c, input);
  if (c == input.size() - 1)
    return res;
  return false;
}

// Metacharacter *
// Matches the preceding element zero or more times
bool anyNumber(size_t &c, const std::string &input, const reFunction &func) {

  while (func(c, input))
    c++;
  return true;
}
} // namespace regex