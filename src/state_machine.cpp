#include <iostream>
#include <regex/state_machine.hpp>
#include <stack>
#include <stdexcept>

namespace regex {

size_t StateMachine::splitNodes(const size_t origin_id,
                                const size_t destination_id,
                                const size_t default_transision) {

  StateNode &origin = states.at(origin_id);
  StateNode &destination = states.at(destination_id);

  states.emplace_back(default_transision);
  size_t new_destination = states.size() - 1;

  for (auto &transision : origin.transisions) {
    if (transision.destination == destination_id)
      transision.destination = new_destination;
  }
}

bool runStateMachine(const StateMachine &engine, const std::string &input) {

  MachineState state(engine.start_state, 0);
  std::stack<MachineState> exec_stack;

  DEBUG_STDOUT("Start run from: " << state.node_id
                                  << " Final: " << engine.final_state << '\n');
  DEBUG_STDOUT(" Input:" << input << '\n')

  while (state.node_id != engine.final_state) {

    if (state.input_id >= input.size())
      if (exec_stack.empty()) {
        break;
      } else {

        state = exec_stack.top();
        exec_stack.pop();
        DEBUG_STDOUT(" recover next: " << state.node_id << ", "
                                       << state.input_id + 1 << '/'
                                       << input.size() << '\n');
      }

    if (state.input_id >= input.size())
      throw std::runtime_error("Run state machine input out of bounds error.");

    char c = input[state.input_id];
    DEBUG_STDOUT("State: " << state.node_id << " c: " << c)
    auto &node = engine.at(state.node_id);
    node.state = state.input_id;
    bool transision_succes = false;
    for (auto transision : node.transisions) {

      if (transision.func(c)) {
        transision_succes = true;
        state.node_id = transision.destination;
        break;
      }
    }

    for (auto transision : node.e_transisions)
      exec_stack.emplace(transision, state.input_id);

    if (!transision_succes) {
      if (exec_stack.empty()) {
        state.node_id = node.default_transision;
      } else {

        auto recovery_state = exec_stack.top();
        exec_stack.pop();
        state.node_id = recovery_state.node_id;
        state.input_id = recovery_state.input_id;
        DEBUG_STDOUT(" pop e = " << recovery_state.node_id << " "
                                 << recovery_state.input_id << '\n');
        continue;
      }
    }

    DEBUG_STDOUT(" next: " << state.node_id << ", " << state.input_id + 1 << '/'
                           << input.size() << '\n');
    state.input_id++;
  }

  DEBUG_STDOUT(" exec_stack : " << exec_stack.size() << "\n")

  return state.node_id == engine.final_state && state.input_id > 0;
}

} // namespace regex