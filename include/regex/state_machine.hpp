#pragma once
#include "functional"
#include "regex/reTree.hpp"
#include "regex/util.hpp"

namespace regex {

typedef std::function<bool /* next_state */ (u_int32_t /* content */)>
    TransisionFunction;

struct Transision {

  Transision(const size_t destination, const TransisionFunction &func)
      : func(func), destination(destination) {}

  Transision(const size_t destination, const TransisionFunction &&func)
      : func(func), destination(destination) {}

  std::string label; /* TODO: only use in debug*/
  TransisionFunction func;
  size_t destination;
};

struct E_Transision {

  E_Transision(const size_t destination) : destination(destination) {}
  E_Transision() : destination(0) {}

  inline bool active() const { return destination != 0; }
  inline size_t get() const { return destination; }
  std::string label; /* TODO: only use in debug*/

  size_t destination;
};

struct MachineState {
  size_t node_id;
  size_t input_id;

  MachineState(size_t state, size_t in) : node_id(state), input_id(in) {}
};

struct StateNode {
  std::vector<Transision> transisions;
  size_t default_transision = 0;

  E_Transision e_transision;

  mutable size_t state; /* TODO: Remove mutable*/

  StateNode(const size_t default_transision)
      : default_transision(default_transision) {}

  StateNode() : default_transision(0) {}

  void addE_transision(const size_t destination) {
    e_transision.destination = destination;
  }

  void pushTransision(const size_t destination,
                      const TransisionFunction &func) {
    transisions.emplace_back(destination, func);
  }

  void pushTransision(const size_t destination,
                      const TransisionFunction &&func) {
    transisions.emplace_back(destination, func);
  }

  void pushTransisionLabel(const std::string &label) {
    transisions.back().label = label;
  }
};

struct StateMachine {

  StateNode &back() { return states.back(); }
  const StateNode &back() const { return states.back(); }
  size_t size() const { return states.size(); }

  StateNode &at(size_t i) { return states.at(i); }
  const StateNode &at(size_t i) const { return states.at(i); }
  size_t splitNodes(const size_t origin, const size_t destination,
                    const size_t default_transision);

  std::vector<StateNode> states;
  size_t start_state;
  size_t final_state;
};

StateMachine buildStateMachineFromTree(const ReTree &tree);

bool runStateMachine(const StateMachine &engine, const std::string &input);

} // namespace regex
