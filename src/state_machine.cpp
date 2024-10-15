#include <iostream>
#include <regex/state_machine.hpp>
#include <stack>
#include <stdexcept>

namespace regex {

StateMachine buildStateMachineFromTree(const ReTree &tree) {

  std::queue<unsigned> tree_node_queue;

  StateMachine state_machine;

  state_machine.start_state = 0;
  state_machine.final_state = 0;

  state_machine.states.emplace_back(0);

  tree_node_queue.push(tree.start_node);

  while (!tree_node_queue.empty()) {

    // Add one more node
    state_machine.states.emplace_back(0);

    const ReNode &current_node = tree.getNode(tree_node_queue.front());
    tree_node_queue.pop();
    size_t next_state_id = state_machine.size();

    switch (current_node.content) {
    case OpCode::ROOT:
      for (unsigned child_id : current_node.children) {
        tree_node_queue.push(child_id);
      }
      break;
    case OpCode::BRACKET:
      for (unsigned child_id : current_node.children) {
        const ReNode &child_node =
            tree.getNode(current_node.children[child_id]);
        if (child_node.content == OpCode::RANGE) {
          throw std::runtime_error("RANGE not implemented");
        } else {
          state_machine.back().pushTransision(
              next_state_id,
              std::bind(isEqual, std::placeholders::_1, child_node.content));
        }
      }
      break;
    case OpCode::KLEENE_STAR:
      throw std::runtime_error("KLEENE_STAR not implemented");
      break;
    case OpCode::RANGE:
      throw std::runtime_error("Invalid RANGE Opcode");
      break;

    default:
      state_machine.back().pushTransision(
          next_state_id,
          std::bind(isEqual, std::placeholders::_1, current_node.content));
      break;
    }
  }

  return state_machine;
}

bool runStateMachine(const StateMachine &engine, const std::string &input) {

  MachineState state(engine.start_state, 0);

  char c = input[state.input_id];
  auto &node = engine.at(state.node_id);

  while (state.input_id < input.size() && state.node_id != engine.final_state) {

    node.state = state.input_id;
    bool transision_succes = false;
    for (auto transision : node.transisions) {

      if (transision.func(c))
        state.node_id = transision.destination;
      break;
    }

    if (!transision_succes)
      state.node_id = node.default_transision;
  }

  return state.node_id == engine.final_state;
}

} // namespace regex