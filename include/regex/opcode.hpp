#pragma once
#include <iostream>
#include <stdint.h>

namespace regex {

enum class OpCode : uint32_t {
  FINAL = 0x10000, // Start at a 32-bit value above 16-bit limit ( Keep 16
                   // bits for Unicode)
  ANY_CHAR,        // Automatically assigned as 0x10001
  BRACKET,         // [abc]
  INV_BRACKET,     // [^abc]
  KLEENE_STAR,     // a*
  START,           // ^abc
  END,             // abc$
  COUNT,           // {4}
  RANGE,           // [ a-b]
  SUBEXPRESSION,   // ()
};

inline uint32_t toInt(OpCode value) { return static_cast<uint32_t>(value); }

// print operator for ReTree content
std::ostream &operator<<(std::ostream &out, const OpCode &value);
} // namespace regex
