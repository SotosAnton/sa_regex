#include <regex/opcode.hpp>
#include <sstream>

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
    case OpCode::UNION:
      out << "|";
      break;
    case OpCode::COUNT:
      out << "{}";
      break;
    case OpCode::UNION_SUBEXPRESION:
      out << "(|)";
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
    case OpCode::LAZY_KLEENE_STAR:
      out << "*?";
      break;
    case OpCode::LAZY_REPETITION:
      out << "+?";
      break;
    case OpCode::LAZY_COUNT:
      out << "{}?";
    case OpCode::LAZY_OPTIONAL:
      out << "??";
      break;
    }
    out << RESET;
  }
  return out;
}

std::string OpCode2String(const OpCode value) {
  std::stringstream out;
  out << value;
  return out.str();
}

std::string OpCode2String(const u_int32_t value) {
  return OpCode2String(static_cast<OpCode>(value));
}

} // namespace regex