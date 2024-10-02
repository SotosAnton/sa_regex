#include <regex/opcode.hpp>

#define ESC "\033["
#define LIGHT_BLUE_BKG "106"
#define PURPLE_TXT "35"
#define RESET "\033[m"
#define MAGENTA "\033[35m" /* Magenta */

namespace regex {
// print operator for ReTree content
std::ostream &operator<<(std::ostream &out, const OpCode &value) {
  out << MAGENTA;
  switch (value) {
  case OpCode::FINAL:
    out << "^";
    break;
  case OpCode::ANY_CHAR:
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
  case OpCode::START:
    out << "^";
    break;
  case OpCode::END:
    out << "$";
    break;
  case OpCode::RANGE:
    out << "-";
    break;
  case OpCode::SUBEXPRESSION:
    out << "()";
    break;
  }
  out << RESET;
  return out;
}
} // namespace regex