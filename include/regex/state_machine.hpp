#pragma once
#include "functional"
#include "regex/reTree.hpp"
#include "regex/util.hpp"

namespace regex {

typedef std::function<bool /* next_state */ (u_int32_t /* content */)>
    TransitionFunction;

struct Transition {

  Transition(const size_t destination, const TransitionFunction &func)
      : func(func), destination(destination) {}

  std::string label; /* TODO: only use in debug*/
  TransitionFunction func;
  size_t destination;
};

struct TransitionFunctor {

  enum Type {
    EQUAL,
    RANGE,
    DIGIT,
    WORD_CHAR,
    NON_DIGIT,
    NON_WORD_CHAR,
    WHITESPACE,
    NON_WHITESPACE,
    WILDCARD
  };

  TransitionFunctor(const size_t destination, const Type type)
      : destination(destination), type(type) {}
  TransitionFunctor(const size_t destination, const Type type,
                    const u_int32_t a)
      : destination(destination), type(type), a(a) {}
  TransitionFunctor(const size_t destination, const Type type,
                    const u_int32_t a, const u_int32_t b)
      : destination(destination), type(type), a(a), b(b) {}

  size_t destination;
  Type type;
  u_int32_t a;

  u_int32_t b;
#ifdef DEBUG
  std::string label;
#endif

  bool operator()(const u_int32_t c) const {
    switch (type) {
    case EQUAL:
      return isEqual(c, a);
    case RANGE:
      return isInRange(c, a, b);
    case DIGIT:
      return isDigit(c);
    case WORD_CHAR:
      return isWordChar(c);
    case NON_DIGIT:
      return isNotDigit(c);
    case NON_WORD_CHAR:
      return isNotWordChar(c);
    case WHITESPACE:
      return isWhitespace(c);
    case NON_WHITESPACE:
      return isNotWhitespace(c);
    case WILDCARD:
      return wildcard(c);
      // default:
      //   std::cerr << " AHA;\n";
      //   throw std::runtime_error("TransitionFunctor invalid operation : " +
      //                            static_cast<char>(c));
    }
  }
};

struct MachineState {
  size_t node_id;
  size_t input_id;

  MachineState(size_t state, size_t in) : node_id(state), input_id(in) {}
  MachineState() : node_id(0), input_id(0) {}
};

struct StateNode {
  // std::vector<Transition> transitions;
  std::vector<TransitionFunctor> transitions;
  size_t default_transition = 0;

  std::vector<size_t> e_transitions;

  StateNode(const size_t default_transition)
      : default_transition(default_transition) {}

  StateNode() : default_transition(0) {}

  void push_E_transition(const size_t destination) {
    e_transitions.push_back(destination);
  }

  void pushTransition(const size_t destination,
                      const TransitionFunctor::Type type) {
    transitions.emplace_back(destination, type);
  }

  void pushTransition(const size_t destination,
                      const TransitionFunctor::Type type, const u_int32_t a) {
    transitions.emplace_back(destination, type, a);
  }
  void pushTransition(const size_t destination,
                      const TransitionFunctor::Type type, const u_int32_t a,
                      const u_int32_t b) {
    transitions.emplace_back(destination, type, a, b);
  }

  void pushTransitionLabel(const std::string &label) {
#ifdef DEBUG
    transitions.back().label = label;
#else
    (void)label;
#endif
  }
};

struct StateMachine {

  StateNode &back() { return states.back(); }
  const StateNode &back() const { return states.back(); }
  size_t size() const { return states.size(); }

  StateNode &at(size_t i) { return states.at(i); }
  const StateNode &at(size_t i) const { return states.at(i); }
  // size_t splitNodes(const size_t origin, const size_t destination,
  //                   const size_t default_transition);

  std::vector<StateNode> states;
  size_t start_state;
  size_t final_state;
};

bool runStateMachine(const StateMachine &engine, const std::string &input,
                     const size_t start_index = 0);

} // namespace regex
