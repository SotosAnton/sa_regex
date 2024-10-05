#pragma once
#include <iostream>
#include <stdint.h>

namespace regex {

enum OpCode : uint32_t {
  ROOT = 0x10000, // Start at a 32-bit value above 16-bit limit ( Keep 16
                  // bits for Unicode)
  FINAL,
  ANY_CHAR,      // Automatically assigned as 0x10001
  BRACKET,       // [abc]
  INV_BRACKET,   // [^abc]
  KLEENE_STAR,   // a*
  AT_START,      // ^abc
  AT_END,        // abc$
  COUNT,         // {4}
  RANGE,         // [ a-b]
  SUBEXPRESSION, // ()
};

// print operator for ReTree content
std::ostream &operator<<(std::ostream &out, const OpCode &value);

std::ostream &printContent(std::ostream &output, const uint32_t &value);

} // namespace regex
