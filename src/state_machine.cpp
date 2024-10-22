#include <iostream>
#include <regex/build_state_machine.hpp>
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

  std::cout << "Start run from: " << state.node_id
            << " Final: " << engine.final_state << '\n';
  std::cout << " Input:" << input << '\n';

  while (state.node_id != engine.final_state) {

    if (state.input_id >= input.size())
      if (exec_stack.empty()) {
        break;
      } else {

        state = exec_stack.top();
        exec_stack.pop();
        std::cout << " recover next: " << state.node_id << ", "
                  << state.input_id + 1 << '/' << input.size() << '\n';
      }

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

    if (node.e_transision.active())
      exec_stack.emplace(node.e_transision.destination, state.input_id);

    if (!transision_succes) {
      if (exec_stack.empty()) {
        state.node_id = node.default_transision;
      } else {

        auto recovery_state = exec_stack.top();
        exec_stack.pop();
        state.node_id = recovery_state.node_id;
        state.input_id = recovery_state.input_id;
        std::cout << " pop e = " << recovery_state.node_id << " "
                  << recovery_state.input_id << '\n';
        continue;
      }
    }

    std::cout << " next: " << state.node_id << ", " << state.input_id + 1 << '/'
              << input.size() << '\n';
    state.input_id++;
  }

  std::cout << " exec_stack : " << exec_stack.size() << "\n";

  return state.node_id == engine.final_state;
}

} // namespace regex