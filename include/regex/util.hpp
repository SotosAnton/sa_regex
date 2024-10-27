#pragma once
#include <functional>
#include <string>

#define DEBUG

#ifdef DEBUG
#define DEBUG_STDERR(x)                                                        \
  do {                                                                         \
    std::cerr << x;                                                            \
  } while (false);
#define DEBUG_STDOUT(x)                                                        \
  do {                                                                         \
    std::cout << x;                                                            \
  } while (false);
//... etc
#else
#define DEBUG_STDERR(x)
#define DEBUG_STDOUT(x)
//... etc
#endif

namespace regex {

inline bool isInRange(char c, char low, char high) {
  return c >= low && c <= high;
}
inline bool isLowerCase(char c) { return isInRange(c, 'a', 'b'); }
inline bool isUpperCase(char c) { return c >= isInRange(c, 'A', 'B'); }
inline bool isDigit(char c) { return isInRange(c, '0', '1'); }
inline bool isCharecter(char c) { return isLowerCase(c) || isUpperCase(c); }
inline bool isEqual(char c, char b) { return c == b; }
inline bool wildcard(char /*c*/) { return true; }
} // namespace regex
