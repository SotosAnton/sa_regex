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

struct MachineState {
  size_t node_id;
  size_t input_id;

  MachineState(size_t state, size_t in) : node_id(state), input_id(in) {}
  MachineState() : node_id(0), input_id(0) {}
};

struct StateNode {
  std::vector<Transition> transitions;
  size_t default_transition = 0;

  std::vector<size_t> e_transitions;

  StateNode(const size_t default_transition)
      : default_transition(default_transition) {}

  StateNode() : default_transition(0) {}

  void push_E_transition(const size_t destination) {
    e_transitions.push_back(destination);
  }

  void pushTransition(const size_t destination,
                      const TransitionFunction &func) {
    transitions.emplace_back(destination, func);
  }

  void pushTransitionLabel(const std::string &label) {
    transitions.back().label = label;
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
