#pragma once
#include "functional"
#include "regex/reTree.hpp"
#include "regex/util.hpp"

namespace regex {

typedef std::function<bool /* next_state */ (u_int32_t /* content */)>
    TransisionFunction;

struct Transision {
  TransisionFunction func;
  size_t destination;
  Transision(const size_t destination, const TransisionFunction &func)
      : func(func), destination(destination) {}

  Transision(const size_t destination, const TransisionFunction &&func)
      : func(func), destination(destination) {}
};

struct MachineState {
  size_t node_id;
  size_t input_id;

  MachineState(size_t state, size_t in) : node_id(state), input_id(in) {}
};

struct StateNode {
  std::vector<Transision> transisions;
  size_t default_transision = 0;

  mutable size_t state; /* TODO: Remove mutable*/

  StateNode(const size_t default_transision)
      : default_transision(default_transision) {}

  StateNode() : default_transision(0) {}

  void pushTransision(const size_t destination,
                      const TransisionFunction &func) {
    transisions.emplace_back(destination, func);
  }

  void pushTransision(const size_t destination,
                      const TransisionFunction &&func) {
    transisions.emplace_back(destination, func);
  }
};

struct StateMachine {
  std::vector<StateNode> states;
  std::stack<MachineState> exec_stack;
  size_t start_state;
  size_t final_state;

  StateNode &back() { return states.back(); }
  size_t size() { return states.size(); }

  StateNode &at(size_t i) { return states.at(i); }
  const StateNode &at(size_t i) const { return states.at(i); }
};

StateMachine buildStateMachineFromTree(const ReTree &tree);

bool runStateMachine(const StateMachine &engine, const std::string &input);

} // namespace regex
