#include <iostream>
#include <regex/state_machine.hpp>
#include <stack>
#include <stdexcept>

namespace regex {

bool runStateMachine(const StateMachine &engine, const std::string &input,
                     const size_t start_index) {

  // MachineState state(engine.start_state, 0);
  std::stack<MachineState> exec_stack;
  exec_stack.emplace(MachineState(engine.start_state, start_index));

  DEBUG_STDOUT("Start run from: " << engine.start_state
                                  << " Final: " << engine.final_state << '\n');
  DEBUG_STDOUT(" Input:" << input << '\n')

#ifdef DEBUG
  unsigned step_counter = 0;
#endif

  MachineState state;

  while (!exec_stack.empty()) {

    state = exec_stack.top();
    exec_stack.pop();

    DEBUG_STDOUT("State: " << state.node_id)
#ifdef DEBUG
    step_counter++;
#endif
    auto &node = engine.at(state.node_id);

    if (state.input_id < input.size()) {

      char c = input[state.input_id];

      DEBUG_STDOUT(" Input : " << state.input_id << " c: " << c << '\n')

      // node.state = state.input_id;
      for (auto transition : node.transitions) {
        if (transition.func(c)) {
          exec_stack.emplace(transition.destination, state.input_id + 1);
          DEBUG_STDOUT(" Valid = " << transition.destination << " "
                                   << state.input_id + 1 << '\n');
        }
      }
    }

    for (auto transition : node.e_transitions) {
      DEBUG_STDOUT(" push e = " << transition << " " << state.input_id << '\n');
      exec_stack.emplace(transition, state.input_id);
    }

    if (state.node_id ==
            engine.final_state /*&& state.input_id == input.size()*/
        && state.input_id > 0) {
      DEBUG_STDERR("Reached final state : " << engine.final_state << " at "
                                            << state.input_id << '\n');
      DEBUG_STDERR("step_counter : " << step_counter << '\n');
      return true;
    }
  }

  DEBUG_STDOUT(" exec_stack : " << exec_stack.size() << "\n")

  return false;
}

} // namespace regex