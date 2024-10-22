#include <deque>
#include <iostream>
#include <regex/state_machine.hpp>
#include <stdexcept>

namespace regex {

using std::to_string;

struct BuildItem {
  size_t tree_node;
  size_t loop_index;
  bool loop_back;

  BuildItem(size_t a) : tree_node(a), loop_index(0), loop_back(false) {}

  BuildItem(size_t a, size_t b, bool c)
      : tree_node(a), loop_index(b), loop_back(c) {}
};

StateMachine buildStateMachineFromTree(const ReTree &tree) {

  std::deque<BuildItem> tree_deque;

  StateMachine state_machine;
  state_machine.states.emplace_back(0);
  state_machine.start_state = 0;
  state_machine.final_state = 0;

  tree_deque.emplace_back(tree.start_node);

  size_t prev_node_id = 0;

  while (!tree_deque.empty()) {

    const BuildItem build_state = tree_deque.front();
    tree_deque.pop_front();

    const ReNode &current_node = tree.getNode(build_state.tree_node);

    size_t next_node_id;
    if (build_state.loop_back) {
      std::cout << "Loop back from: " << prev_node_id << " to "
                << build_state.loop_index << " empty : " << '\n';
      StateNode &loop_node = state_machine.states.at(prev_node_id);

      std::cout << "loop_node.transisions.empty() "
                << loop_node.transisions.empty() << '\n';
      if (loop_node.transisions.empty()) {
        loop_node.default_transision = build_state.loop_index;
        loop_node.e_transision.destination = build_state.loop_index;
      }
      for (auto &transision : loop_node.transisions) {
        transision.destination = build_state.loop_index;
      }
      prev_node_id = build_state.loop_index + 1;
      continue;
    }

    next_node_id = state_machine.size();

    std::cout << "Adding State: "
              << static_cast<regex::OpCode>(current_node.content) << " from "
              << prev_node_id << " to  " << next_node_id << '\n';

    switch (current_node.content) {
    case OpCode::ROOT:
      for (unsigned child_id : current_node.children) {
        tree_deque.emplace_back(child_id);
      }
      break;
    case OpCode::BRACKET:
      state_machine.states.emplace_back(0);
      for (unsigned child_id : current_node.children) {
        const ReNode &child_node = tree.getNode(child_id);
        if (child_node.content == OpCode::RANGE) {
          auto r_min = tree.getNode(child_node.children[0]).content;
          auto r_max = tree.getNode(child_node.children[1]).content;

          state_machine.at(prev_node_id)
              .pushTransision(
                  next_node_id,
                  std::bind(isInRange, std::placeholders::_1, r_min, r_max));

          state_machine.at(prev_node_id)
              .pushTransisionLabel(std::string(1, static_cast<char>(r_min)) +
                                   " - " +
                                   std::string(1, static_cast<char>(r_max)));
        } else {
          state_machine.at(prev_node_id)
              .pushTransision(next_node_id,
                              std::bind(isEqual, std::placeholders::_1,
                                        child_node.content));
          state_machine.at(prev_node_id)
              .pushTransisionLabel(
                  " = " +
                  std::string(1, static_cast<char>(child_node.content)));
        }
      }
      break;
    case OpCode::KLEENE_STAR:

      tree_deque.emplace_front(0, prev_node_id, true);

      state_machine.states.emplace_back(0);

      state_machine.states.at(prev_node_id).e_transision.destination =
          next_node_id;

      next_node_id = prev_node_id;

      for (std::vector<int>::const_reverse_iterator riter =
               current_node.children.rbegin();
           riter != current_node.children.rend(); ++riter) {
        tree_deque.push_front(*riter);
      }

      break;
    case OpCode::RANGE:
      throw std::runtime_error("Invalid RANGE Opcode");
      break;
    case OpCode::SUBEXPRESSION:
      for (unsigned child_id : current_node.children) {
        tree_deque.emplace_back(child_id);
      }
      break;
    default:
      state_machine.states.emplace_back(0);
      std::cout << "Add Transision  = "
                << static_cast<regex::OpCode>(current_node.content) << " to "
                << next_node_id << '\n';
      state_machine.at(prev_node_id)
          .pushTransision(
              next_node_id,
              std::bind(isEqual, std::placeholders::_1, current_node.content));
      state_machine.at(prev_node_id)
          .pushTransisionLabel(
              " = " + std::string(1, static_cast<char>(current_node.content)));
      break;
    }
    if (next_node_id != state_machine.size())
      prev_node_id = next_node_id;
  }
  state_machine.final_state = state_machine.size() - 1;
  return state_machine;
}
} // namespace regex