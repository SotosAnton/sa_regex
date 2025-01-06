#pragma once
#include "functional"
#include "regex/reTree.hpp"
#include "regex/util.hpp"
#include <regex/state_machine.hpp>

namespace regex {

struct MachineStateHash {

  size_t operator()(const MachineState &t) const {
    return std::hash<size_t>()(t.input_id) ^
           (std::hash<size_t>()(t.node_id) << 1);
  }
};

typedef std::unordered_set<MachineState, MachineStateHash> StateContainer;

inline bool operator==(const MachineState &t1, const MachineState &t2) {
  return t1.input_id == t2.input_id && t1.node_id == t2.node_id;
}

class StateMachineExecutor {
public:
  StateMachineExecutor(const StateMachine *engine) : engine(engine) {
    state_set.resize(engine->size());
  }

  StateMachineExecutor() { engine = nullptr; }
  bool run(const std::string input,
           std::vector<std::pair<size_t, size_t>> *matches = nullptr);

private:
  void depthFirstSearch(const MachineState &start_state,
                        StateContainer *state_container);

  bool runStateMachineSmart(const std::string &input, const size_t start_index,
                            size_t *end_index = nullptr);

  bool runStateMachine(const std::string &input, const size_t start_index);
  const StateMachine *engine;

  StateContainer set_1;
  StateContainer set_2;

  std::vector<std::vector<size_t>> state_set;
};

} // namespace regex
