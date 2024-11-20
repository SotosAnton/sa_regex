#include <deque>
#include <iostream>
#include <regex/builder.hpp>
#include <regex/state_machine.hpp>
#include <stdexcept>

namespace regex {

using std::to_string;

#define BUILD_MAP_ITEM(code, func)                                             \
  {                                                                            \
    code, [](StateMachineBuilder *self, const ReNode &node,                    \
             const BuildItem &state, size_t &prev,                             \
             size_t &next) { self->func(node, state, prev, next); }            \
  }

#define BUILD_MAP_TROW(code)                                                   \
  {                                                                            \
    code,                                                                      \
        [](StateMachineBuilder *, const ReNode &, const BuildItem &, size_t &, \
           size_t &) { throw std::runtime_error("Invalid Opcode: " #code); }   \
  }

#define BUILD_MAP_ITEM_SIMPLE(code, func, label)                               \
  {                                                                            \
    code, [](StateMachineBuilder *self, const ReNode &node,                    \
             const BuildItem &state, size_t &prev, size_t &next) {             \
      self->build_simple_node(node, state, prev, next,                         \
                              std::bind(func, std::placeholders::_1), label);  \
    }                                                                          \
  }

const std::unordered_map<OpCode, StateMachineBuilder::BuildFunction>
    StateMachineBuilder::build_map = {
        BUILD_MAP_ITEM(OpCode::ROOT, build_ROOT),
        BUILD_MAP_ITEM(OpCode::BRACKET, build_BRACKET),
        BUILD_MAP_ITEM(OpCode::KLEENE_STAR, build_KLEENE_STAR),
        BUILD_MAP_ITEM(OpCode::REPETITION, build_REPETITION),
        BUILD_MAP_ITEM(OpCode::SUBEXPRESSION, build_ROOT),
        BUILD_MAP_ITEM(OpCode::OPTIONAL, build_OPTIONAL),
        BUILD_MAP_ITEM(OpCode::UNION, build_UNION),
        BUILD_MAP_ITEM(OpCode::UNION_SUBEXPRESION, build_UNION_SUBEXPRESION),
        BUILD_MAP_ITEM(OpCode::AT_START, build_AT_START),
        BUILD_MAP_ITEM(OpCode::AT_END, build_AT_END),
        BUILD_MAP_ITEM(OpCode::AT_END, build_AT_END),
        BUILD_MAP_ITEM_SIMPLE(OpCode::WILDCARD, wildcard, " =  . "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::DIGIT, isDigit, " =  \\d "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::WORD_CHAR, isWordChar, " =  \\w "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::NON_DIGIT, isNotDigit, " =  \\D "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::NON_WORD_CHAR, isNotWordChar, " =  \\W "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::WHITESPACE, isWhitespace, " =  \\s "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::NON_WHITESPACE, isNotWhitespace,
                              " =  \\S "),
        BUILD_MAP_TROW(OpCode::RANGE),

};

void StateMachineBuilder::build_Node(const ReNode &current_node,
                                     const BuildItem &build_state,
                                     size_t &prev_node_id,
                                     size_t &next_node_id) {
  auto search = build_map.find(static_cast<OpCode>(current_node.content));
  if (search == build_map.end()) {
    build_simple_node(
        current_node, build_state, prev_node_id, next_node_id,
        std::bind(isEqual, std::placeholders::_1, current_node.content),
        " = " + std::string(1, static_cast<char>(current_node.content)));
  } else {
    search->second(this, current_node, build_state, prev_node_id, next_node_id);
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

    next_node_id = state_machine.size();

    DEBUG_STDOUT("Adding State: "
                 << static_cast<regex::OpCode>(current_node.content) << " from "
                 << prev_node_id << " to  " << next_node_id << '\n');

    build_Node(current_node, build_state, prev_node_id, next_node_id);

    if (next_node_id != state_machine.size())
      prev_node_id = next_node_id;
  }
  state_machine.final_state = prev_node_id;
  state_machine.at(state_machine.final_state).default_transition =
      state_machine.final_state;

  return std::move(state_machine);
}

void StateMachineBuilder::build_ROOT(const ReNode &current_node,
                                     const BuildItem &build_state,
                                     size_t & /*prev_node_id*/,
                                     size_t & /*next_node_id*/) {
#ifdef DEBUG
  if (!build_state.entering)
    throw std::runtime_error("Root node exit.");
#endif

  for (auto it = current_node.children.rbegin();
       it != current_node.children.rend(); ++it) {
    tree_deque.emplace_back(*it);
  }
}

void StateMachineBuilder::build_BRACKET(const ReNode &current_node,
                                        const BuildItem & /*build_state*/,
                                        size_t &prev_node_id,
                                        size_t &next_node_id) {
  state_machine.states.emplace_back(0);
  for (unsigned child_id : current_node.children) {
    const ReNode &child_node = tree->getNode(child_id);
    if (child_node.content == OpCode::RANGE) {
      auto r_min = tree->getNode(child_node.children[0]).content;
      auto r_max = tree->getNode(child_node.children[1]).content;

      state_machine.at(prev_node_id)
          .pushTransition(
              next_node_id,
              std::bind(isInRange, std::placeholders::_1, r_min, r_max));

      state_machine.at(prev_node_id)
          .pushTransitionLabel(std::string(1, static_cast<char>(r_min)) +
                               " - " +
                               std::string(1, static_cast<char>(r_max)));
    } else {
      state_machine.at(prev_node_id)
          .pushTransition(
              next_node_id,
              std::bind(isEqual, std::placeholders::_1, child_node.content));
      state_machine.at(prev_node_id)
          .pushTransitionLabel(
              " = " + std::string(1, static_cast<char>(child_node.content)));
    }
  }
}

void StateMachineBuilder::build_REPETITION(const ReNode &current_node,
                                           const BuildItem &build_state,
                                           size_t &prev_node_id,
                                           size_t &next_node_id) {
  if (build_state.entering) {
    state_machine.states.emplace_back(0);

    prev_node_id = state_machine.size() - 2;
    next_node_id = prev_node_id;

    // emplace_back KLEENE_STAR as exit node to close loop
    tree_deque.emplace_back(build_state.tree_node, state_machine.size() - 1,
                            false);

    for (auto riter = current_node.children.rbegin();
         riter != current_node.children.rend(); ++riter) {
      tree_deque.emplace_back(*riter);
    }

  } else {

    StateNode &loop_node = state_machine.states.at(prev_node_id);
    loop_node.push_E_transition(build_state.state_machine_node);
    loop_node.push_E_transition(build_state.state_machine_node - 1);
    prev_node_id = build_state.state_machine_node;
  }
}

void StateMachineBuilder::build_KLEENE_STAR(const ReNode &current_node,
                                            const BuildItem &build_state,
                                            size_t &prev_node_id,
                                            size_t &next_node_id) {
  if (build_state.entering) {

    state_machine.states.back().push_E_transition(next_node_id);
    state_machine.states.back().push_E_transition(next_node_id + 1);

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

  } else {

    StateNode &loop_node = state_machine.states.at(prev_node_id);
    loop_node.push_E_transition(build_state.state_machine_node);
    loop_node.push_E_transition(build_state.state_machine_node + 1);
    prev_node_id = build_state.state_machine_node;
  }
}

// void StateMachineBuilder::build_RANGE(const ReNode &current_node,
//                                       const BuildItem &build_state,
//                                       size_t &prev_node_id,
//                                       size_t &next_node_id) {}

void StateMachineBuilder::build_OPTIONAL(const ReNode &current_node,
                                         const BuildItem &build_state,
                                         size_t &prev_node_id,
                                         size_t & /*next_node_id */) {
  if (build_state.entering) {
    // emplace_back OPTIONAL as exit node to close loop
    tree_deque.emplace_back(build_state.tree_node, state_machine.size() - 1,
                            false);

    for (auto riter = current_node.children.rbegin();
         riter != current_node.children.rend(); ++riter) {
      tree_deque.emplace_back(*riter);
    }
  } else {

    state_machine.states.at(build_state.state_machine_node)
        .push_E_transition(prev_node_id);
  }
}

void StateMachineBuilder::build_AT_START(const ReNode &current_node,
                                         const BuildItem & /*build_state*/,
                                         size_t & /*prev_node_id*/,
                                         size_t &next_node_id) {
  state_machine.states.emplace_back(0);
  state_machine.start_state = 1;

  for (auto riter = current_node.children.rbegin();
       riter != current_node.children.rend(); ++riter) {
    tree_deque.emplace_back(*riter);
  }
}

void StateMachineBuilder::build_AT_END(const ReNode &current_node,
                                       const BuildItem & /*build_state*/,
                                       size_t & /*prev_node_id*/,
                                       size_t &next_node_id) {
  state_machine.states.emplace_back(0);
  state_machine.start_state = 1;

  for (auto riter = current_node.children.rbegin();
       riter != current_node.children.rend(); ++riter) {
    tree_deque.emplace_back(*riter);
  }
}

void StateMachineBuilder::build_UNION(const ReNode &current_node,
                                      const BuildItem &build_state,
                                      size_t &prev_node_id,
                                      size_t &next_node_id) {
  if (build_state.entering) {

    state_machine.states.emplace_back(0);
    // emplace_back UNION as exit node to close loop
    tree_deque.emplace_back(build_state.tree_node, state_machine.size() - 1,
                            false);

    for (auto riter = current_node.children.rbegin();
         riter != current_node.children.rend(); ++riter) {
      // state_/machine.at(prev_node_id)
      // .push_E_transition(state_machine.size() - 1);
      tree_deque.emplace_back(*riter, state_machine.size() - 1, true);
    }
  } else {

    prev_node_id = build_state.state_machine_node;
  }
}

void StateMachineBuilder::build_UNION_SUBEXPRESION(const ReNode &current_node,
                                                   const BuildItem &build_state,
                                                   size_t &prev_node_id,
                                                   size_t &next_node_id) {
  if (build_state.entering) {

    // emplace_back UNION as exit node to close loop
    state_machine.states.emplace_back(0);
    state_machine.states.at(build_state.state_machine_node - 1)
        .push_E_transition(state_machine.size() - 1);

    tree_deque.emplace_back(build_state.tree_node,
                            build_state.state_machine_node, false);
    prev_node_id = build_state.state_machine_node - 1;

    // state_machine.states.at(build_state.state_machine_node)
    //     .push_E_transition(prev_node_id);

    for (auto riter = current_node.children.rbegin();
         riter != current_node.children.rend(); ++riter) {
      tree_deque.emplace_back(*riter, prev_node_id, true);
    }
  } else {

    state_machine.states.at(prev_node_id)
        .push_E_transition(build_state.state_machine_node);
    prev_node_id = build_state.state_machine_node;
  }
}

void StateMachineBuilder::build_simple_node(
    const ReNode &current_node, const BuildItem &build_state,
    size_t &prev_node_id, size_t &next_node_id,
    const regex::TransitionFunction &func, const std::string &label) {

#ifdef DEBUG
  if (!build_state.entering)
    throw std::runtime_error("build_simple_node exit.");
#endif

  state_machine.states.emplace_back(0);
  DEBUG_STDOUT("Add Transition  = "
               << static_cast<regex::OpCode>(current_node.content) << " to "
               << next_node_id << '\n');
  state_machine.at(prev_node_id).pushTransition(next_node_id, func);
  state_machine.at(prev_node_id).pushTransitionLabel(label /* " =  \\s " */);
}

StateMachine buildStateMachineFromTree(const ReTree &tree) {
  StateMachineBuilder builder(&tree);
  return builder.build();
}

} // namespace regex