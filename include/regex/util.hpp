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
// Symbol class fucntions
inline bool isInRange(char c, char low, char high) {
  return c >= low && c <= high;
}
inline bool isLowerCase(char c) { return isInRange(c, 'a', 'z'); }
inline bool isUpperCase(char c) { return isInRange(c, 'A', 'Z'); }
inline bool isDigit(char c) { return isInRange(c, '0', '9'); }
inline bool isNotDigit(char c) { return !isDigit(c); }

inline bool isCharacter(char c) { return isLowerCase(c) || isUpperCase(c); }
inline bool isEqual(char c, char b) { return c == b; }
inline bool isWordChar(char c) {
  return isCharacter(c) || isDigit(c) || isEqual(c, '_');
}
inline bool isNotWordChar(char c) { return !isWordChar(c); }
inline bool isWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' ||
         c == '\r';
}
inline bool isNotWhitespace(char c) { return !isWhitespace(c); }
inline bool wildcard(char /*c*/) { return true; }

// Counter Functions
inline bool less(u_int32_t in, u_int32_t c) { return in < c; }
inline bool greater(u_int32_t in, u_int32_t c) { return in > c; }
inline bool lessEq(u_int32_t in, u_int32_t c) { return in <= c; }
inline bool greaterEq(u_int32_t in, u_int32_t c) { return in >= c; }
inline bool range(u_int32_t in, u_int32_t low, u_int32_t high) {
  return in > low && in < high;
}
inline bool rangeEq(u_int32_t in, u_int32_t low, u_int32_t high) {
  return in >= low && in <= high;
}

} // namespace regex
