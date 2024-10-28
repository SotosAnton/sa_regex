#include <regex/opcode.hpp>

#define RESET "\033[m"
#define MAGENTA "\033[35m" /* Magenta */

namespace regex {
// print operator for ReTree content
std::ostream &operator<<(std::ostream &out, const OpCode &value) {

  if (value < 0x10000) {
    out << static_cast<char>(value);
  } else {
    out << MAGENTA;
    switch (value) {
    case OpCode::ROOT:
      out << "@";
      break;
    case OpCode::FINAL:
      out << "^";
      break;
    case OpCode::WILDCARD:
      out << ".";
      break;
    case OpCode::BRACKET:
      out << "[]";
      break;
    case OpCode::INV_BRACKET:
      out << "[^]";
      break;
    case OpCode::KLEENE_STAR:
      out << "*";
      break;
    case OpCode::AT_START:
      out << "^";
      break;
    case OpCode::AT_END:
      out << "$";
      break;
    case OpCode::RANGE:
      out << "-";
      break;
    case OpCode::SUBEXPRESSION:
      out << "()";
      break;
    case OpCode::REPETITION:
      out << "+";
      break;
    case OpCode::OPTIONAL:
      out << "?";
      break;
    case OpCode::WHITESPACE:
      out << "\\s";
      break;
    case OpCode::NON_WHITESPACE:
      out << "\\S";
      break;
    case OpCode::WORD_CHAR:
      out << "\\w";
      break;
    case OpCode::NON_WORD_CHAR:
      out << "\\W";
      break;
    case OpCode::DIGIT:
      out << "\\d";
      break;
    case OpCode::NON_DIGIT:
      out << "\\D";
      break;
    }
    out << RESET;
  }
  return out;
}

} // namespace regex