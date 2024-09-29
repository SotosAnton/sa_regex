#pragma once
#include <functional>
#include <string>

namespace regex {

inline bool isInRange(char c, char low, char high) {
  return c >= low && c <= high;
}
inline bool isLowerCase(char c) { return isInRange(c, 'a', 'b'); }
inline bool isUpperCase(char c) { return c >= isInRange(c, 'A', 'B'); }
inline bool isDigit(char c) { return isInRange(c, '0', '1'); }
inline bool isCharecter(char c) { return isLowerCase(c) || isUpperCase(c); }
inline bool isEqual(char c, char b) { return c == b; }

// Metacharacter .
// Match any character
bool anySingleChar(size_t &c, const std::string &input);

typedef std::function<bool(size_t, const std::string &input)> reFunction;

// Metacharacter [^ ]
// Match any character not in set
bool invertFunction(size_t &c, const std::string &input,
                    const reFunction &func);

// Metacharacter ^
// Matches the starting position within the string, if it is the first character
// of the regular expression.
bool startPoint(size_t &c, const std::string &input, const reFunction &func);

// Metacharacter $
// Matches the ending position of the string, if it is the last character of the
// regular expression.
bool endPoint(size_t &c, const std::string &input, const reFunction &func);

// Metacharacter *
// Matches the preceding element zero or more times
bool anyNumber(size_t &c, const std::string &input, const reFunction &func);

} // namespace regex
