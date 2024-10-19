#include <iostream>
#include <regex/build_state_machine.hpp>
#include <stack>
#include <stdexcept>

namespace regex {

bool runStateMachine(const StateMachine &engine, const std::string &input) {

  MachineState state(engine.start_state, 0);
  std::stack<MachineState> exec_stack;

  std::cout << "Start run from: " << state.node_id
            << " Final: " << engine.final_state << '\n';
  std::cout << " Input:" << input << '\n';

  while (state.input_id < input.size() && state.node_id != engine.final_state) {

    char c = input[state.input_id];
    std::cout << "State: " << state.node_id << " c: " << c;
    auto &node = engine.at(state.node_id);
    node.state = state.input_id;
    bool transision_succes = false;
    for (auto transision : node.transisions) {

      bool eval = transision.func(c);
      std::cout << " func eval = " << eval << '\n';
      if (eval) {
        transision_succes = true;
        state.node_id = transision.destination;
        break;
      }
    }

    if (!transision_succes) {
      if (exec_stack.empty()) {
        state.node_id = node.default_transision;
      } else {
        auto recovery_state = exec_stack.top();
        exec_stack.pop();
        state.node_id = recovery_state.node_id;
        state.input_id = recovery_state.input_id;
      }
    }

    std::cout << " next: " << state.node_id << '\n';
    state.input_id++;
  }

  return state.node_id == engine.final_state;
}

} // namespace regex