#pragma once
#include <iostream>
#include <stdint.h>

namespace regex {

enum OpCode : uint32_t {
  ROOT = 0x10000, // Start at a 32-bit value above 16-bit limit ( Keep 16
                  // bits for Unicode)
  FINAL,          // Automatically assigned as 0x10001
  WILDCARD,
  BRACKET,        // [abc]
  INV_BRACKET,    // [^abc]
  KLEENE_STAR,    // a*
  AT_START,       // ^abc
  AT_END,         // abc$
  COUNT,          // {4}
  RANGE,          // [ a-b]
  SUBEXPRESSION,  // ()
  UNION,          // |
  OPTIONAL,       // ? match 1 or 0
  REPETITION,     // +  at least one
  AT_LEAST,       // {5,} match at least 5
  RANGE_COUNT,    // {5,10} match count > 5 and < 10
  WHITESPACE,     // \s
  NON_WHITESPACE, // \S
  WORD_CHAR,      // \w
  NON_WORD_CHAR,  // \W
  DIGIT,          // \d
  NON_DIGIT,      // \D

};

std::string OpCode2String(const OpCode value);
std::string OpCode2String(const u_int32_t value);
// print operator for ReTree content
std::ostream &operator<<(std::ostream &out, const OpCode &value);

} // namespace regex
