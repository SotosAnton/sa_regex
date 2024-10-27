#include <deque>
#include <iostream>
#include <regex/builder.hpp>
#include <regex/state_machine.hpp>
#include <stdexcept>

namespace regex {

using std::to_string;

void StateMachineBuilder::nodeEntry(const ReNode &current_node,
                                    const BuildItem &build_state,
                                    size_t &prev_node_id,
                                    size_t &next_node_id) {

  switch (current_node.content) {
  case OpCode::ROOT:

    for (auto it = current_node.children.rbegin();
         it != current_node.children.rend(); ++it) {
      tree_deque.emplace_back(*it);
    }

    break;
  case OpCode::BRACKET:
    state_machine.states.emplace_back(0);
    for (unsigned child_id : current_node.children) {
      const ReNode &child_node = tree->getNode(child_id);
      if (child_node.content == OpCode::RANGE) {
        auto r_min = tree->getNode(child_node.children[0]).content;
        auto r_max = tree->getNode(child_node.children[1]).content;

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
            .pushTransision(
                next_node_id,
                std::bind(isEqual, std::placeholders::_1, child_node.content));
        state_machine.at(prev_node_id)
            .pushTransisionLabel(
                " = " + std::string(1, static_cast<char>(child_node.content)));
      }
    }
    break;
  case OpCode::KLEENE_STAR:

    state_machine.states.back().push_E_transision(next_node_id);
    state_machine.states.back().push_E_transision(next_node_id + 1);

    state_machine.states.emplace_back(0);

    state_machine.states.emplace_back(0);

    prev_node_id = state_machine.size() - 1;
    next_node_id = prev_node_id;

    // BuildItem(size_t tree_node, size_t state_machine_node, bool entering)

    // emplace_back KLEENE_STAR as exit node to close loop
    tree_deque.emplace_back(build_state.tree_node, state_machine.size() - 2,
                            false);

    for (auto riter = current_node.children.rbegin();
         riter != current_node.children.rend(); ++riter) {
      tree_deque.emplace_back(*riter);
    }
    return;
    // break;
  case OpCode::REPETITION:

    // state_machine.states.back().push_E_transision(next_node_id);
    // state_machine.states.back().push_E_transision(next_node_id + 1);

    state_machine.states.emplace_back(0);

    // state_machine.states.emplace_back(0);

    prev_node_id = state_machine.size() - 2;
    next_node_id = prev_node_id;

    tree_deque.emplace_back(state_machine.size() - 2, /* loop back*/
                            state_machine.size() - 1 /* loop next*/, false);

    for (auto riter = current_node.children.rbegin();
         riter != current_node.children.rend(); ++riter) {
      tree_deque.emplace_back(*riter);
    }
    // 5
    return;
    // break;
  case OpCode::RANGE:
    throw std::runtime_error("Invalid RANGE Opcode");
    break;
  case OpCode::SUBEXPRESSION:
    for (std::vector<int>::const_reverse_iterator riter =
             current_node.children.rbegin();
         riter != current_node.children.rend(); ++riter) {
      tree_deque.emplace_back(*riter);
    }
    break;
  case OpCode::WILDCARD:
    state_machine.states.emplace_back(0);
    state_machine.at(prev_node_id)
        .pushTransision(next_node_id,
                        std::bind(wildcard, std::placeholders::_1));
    state_machine.at(prev_node_id).pushTransisionLabel(" = . ");
    break;
  case OpCode::OPTIONAL:
    state_machine.states.emplace_back(0);
    state_machine.at(prev_node_id)
        .pushTransision(next_node_id,
                        std::bind(wildcard, std::placeholders::_1));
    state_machine.at(prev_node_id).pushTransisionLabel(" = . ");
    break;
  default:
    state_machine.states.emplace_back(0);
    DEBUG_STDOUT("Add Transision  = "
                 << static_cast<regex::OpCode>(current_node.content) << " to "
                 << next_node_id << '\n');
    state_machine.at(prev_node_id)
        .pushTransision(next_node_id, std::bind(isEqual, std::placeholders::_1,
                                                current_node.content));
    state_machine.at(prev_node_id)
        .pushTransisionLabel(
            " = " + std::string(1, static_cast<char>(current_node.content)));
    break;
  }
}

StateMachine StateMachineBuilder::build() {

  state_machine.states.emplace_back(0);
  state_machine.start_state = 0;
  state_machine.final_state = 0;

  tree_deque.emplace_back(tree->start_node);

  size_t prev_node_id = 0;

  while (!tree_deque.empty()) {

    const BuildItem build_state = tree_deque.back();
    tree_deque.pop_back();

    const ReNode &current_node = tree->getNode(build_state.tree_node);

    size_t next_node_id;

    DEBUG_STDOUT("Adding State: "
                 << static_cast<regex::OpCode>(current_node.content) << " from "
                 << prev_node_id << " to  " << next_node_id << '\n');

    // if (!build_state.entering) {
    //   DEBUG_STDOUT("Loop back from: " << prev_node_id << " to "
    //                                   << build_state.state_machine_node
    //                                   << " empty : " << '\n');
    //   StateNode &loop_node = state_machine.states.at(prev_node_id);

    //   // Back to start of loop
    //   // if (build_state.tree_node != 0)
    //   loop_node.push_E_transision(build_state.tree_node);

    //   // Next state to get out of loop

    //   // if (build_state.state_machine_node != 0)
    //   loop_node.push_E_transision(build_state.state_machine_node);

    //   prev_node_id = build_state.state_machine_node;
    //   continue;
    // }

    next_node_id = state_machine.size();
    if (build_state.entering)
      nodeEntry(current_node, build_state, prev_node_id, next_node_id);
    else
      nodeExit(current_node, build_state, prev_node_id, next_node_id);

    if (next_node_id != state_machine.size())
      prev_node_id = next_node_id;
  }
  state_machine.final_state = state_machine.size() - 1;

  return std::move(state_machine);
}

void StateMachineBuilder::nodeExit(const ReNode &current_node,
                                   const BuildItem &build_state,
                                   size_t &prev_node_id, size_t &next_node_id) {
  DEBUG_STDOUT("Node exit -> State: "
               << static_cast<regex::OpCode>(current_node.content) << " from "
               << prev_node_id << " to  " << next_node_id << '\n');

  switch (current_node.content) {
  case OpCode::KLEENE_STAR:

    StateNode &loop_node = state_machine.states.at(prev_node_id);

    loop_node.push_E_transision(build_state.state_machine_node);
    loop_node.push_E_transision(build_state.state_machine_node + 1);
    prev_node_id = build_state.state_machine_node;

    return;
  }
}

StateMachine buildStateMachineFromTree(const ReTree &tree) {
  StateMachineBuilder builder(&tree);
  return builder.build();
}

} // namespace regex