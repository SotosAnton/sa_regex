#include <iostream>
#include <regex/state_machine.hpp>
#include <stack>
#include <stdexcept>

namespace regex {

using std::to_string;

StateMachine buildStateMachineFromTree(const ReTree &tree) {

  std::queue<unsigned> tree_node_queue;

  StateMachine state_machine;

  state_machine.start_state = 0;
  state_machine.final_state = 0;

  tree_node_queue.push(tree.start_node);

  while (!tree_node_queue.empty()) {

    // Add one more node

    const ReNode &current_node = tree.getNode(tree_node_queue.front());
    tree_node_queue.pop();
    size_t next_state_id = state_machine.size() + 1;

    std::cout << "Adding State: "
              << static_cast<regex::OpCode>(current_node.content) << '\n';

    switch (current_node.content) {
    case OpCode::ROOT:
      for (unsigned child_id : current_node.children) {
        tree_node_queue.push(child_id);
      }
      break;
    case OpCode::BRACKET:
      state_machine.states.emplace_back(0);
      for (unsigned child_id : current_node.children) {
        const ReNode &child_node = tree.getNode(child_id);
        if (child_node.content == OpCode::RANGE) {
          auto r_min = tree.getNode(child_node.children[0]).content;
          auto r_max = tree.getNode(child_node.children[1]).content;
          state_machine.back().pushTransision(
              next_state_id,
              std::bind(isInRange, std::placeholders::_1, r_min, r_max));
          state_machine.back().pushTransisionLabel(
              std::string(1, static_cast<char>(r_min)) + " - " +
              std::string(1, static_cast<char>(r_max)));
        } else {
          state_machine.back().pushTransision(
              next_state_id,
              std::bind(isEqual, std::placeholders::_1, child_node.content));
          state_machine.back().pushTransisionLabel(
              " = " + std::string(1, static_cast<char>(child_node.content)));
        }
      }
      break;
    case OpCode::KLEENE_STAR:
      state_machine.states.emplace_back(state_machine.size());
      for (unsigned child_id : current_node.children) {
        tree_node_queue.push(child_id);
      }
      break;
    case OpCode::RANGE:
      throw std::runtime_error("Invalid RANGE Opcode");
      break;

    default:
      state_machine.states.emplace_back(0);
      std::cout << "Add Transision  = "
                << static_cast<regex::OpCode>(current_node.content) << " to "
                << next_state_id << '\n';
      state_machine.back().pushTransision(
          next_state_id,
          std::bind(isEqual, std::placeholders::_1, current_node.content));
      state_machine.back().pushTransisionLabel(" = " +
                                               to_string(current_node.content));
      break;
    }
  }

  state_machine.final_state = state_machine.size();
  return state_machine;
}
} // namespace regex