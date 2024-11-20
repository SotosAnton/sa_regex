#include <iostream>
#include <regex/state_machine.hpp>
#include <stack>
#include <stdexcept>

namespace regex {

// size_t StateMachine::splitNodes(const size_t origin_id,
//                                 const size_t destination_id,
//                                 const size_t default_transition) {

//   StateNode &origin = states.at(origin_id);
//   StateNode &destination = states.at(destination_id);

//   states.emplace_back(default_transition);
//   size_t new_destination = states.size() - 1;

//   for (auto &transition : origin.transitions) {
//     if (transition.destination == destination_id)
//       transition.destination = new_destination;
//   }
// }

bool runStateMachine(const StateMachine &engine, const std::string &input) {

  // MachineState state(engine.start_state, 0);
  std::stack<MachineState> exec_stack;
  exec_stack.emplace(MachineState(engine.start_state, 0));

  DEBUG_STDOUT("Start run from: " << engine.start_state
                                  << " Final: " << engine.final_state << '\n');
  DEBUG_STDOUT(" Input:" << input << '\n')

  while (!exec_stack.empty()) {

    MachineState state = exec_stack.top();
    exec_stack.pop();

    DEBUG_STDOUT("State: " << state.node_id)
    auto &node = engine.at(state.node_id);

    bool transisition_success = false;

    if (state.input_id < input.size()) {

      char c = input[state.input_id];

      DEBUG_STDOUT(" Input : " << state.node_id << " c: " << c << '\n')

      // trap node
      if (node.transitions.empty() && node.e_transitions.empty()) {
        if (state.node_id == engine.final_state)
          return true;
        else
          return false;
      }

      // node.state = state.input_id;
      for (auto transition : node.transitions) {
        if (transition.func(c)) {
          exec_stack.emplace(transition.destination, state.input_id + 1);
          transisition_success = true;
          DEBUG_STDOUT(" Valid = " << transition.destination << " "
                                   << state.input_id + 1 << '\n');
        }
      }
    }

    for (auto transition : node.e_transitions) {
      DEBUG_STDOUT(" push e = " << transition << " " << state.input_id << '\n');
      exec_stack.emplace(transition, state.input_id);
      transisition_success = true;
    }

    if (!transisition_success && (state.input_id + 1) < input.size())
      exec_stack.emplace(node.default_transition, state.input_id + 1);

    // if (!transition_succes) {
    //   if (exec_stack.empty()) {
    //     state.node_id = node.default_transition;
    //   } else {

    //     auto recovery_state = exec_stack.top();
    //     exec_stack.pop();
    //     state.node_id = recovery_state.node_id;
    //     state.input_id = recovery_state.input_id;
    //     DEBUG_STDOUT(" pop e = " << recovery_state.node_id << " "
    //                              << recovery_state.input_id << '\n');
    //     continue;
    //   }
    // }

    if (state.node_id == engine.final_state)
      return true;
  }

  DEBUG_STDOUT(" exec_stack : " << exec_stack.size() << "\n")

  return false;
}

} // namespace regex