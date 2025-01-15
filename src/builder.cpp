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
             const BuildItem &state) { self->func(node, state); }              \
  }

#define BUILD_MAP_THROW(code)                                                  \
  {                                                                            \
    code, [](StateMachineBuilder *, const ReNode &, const BuildItem &) {       \
      throw std::runtime_error("Invalid Opcode: " #code);                      \
    }                                                                          \
  }

#define BUILD_MAP_ITEM_SIMPLE(code, type, label)                               \
  {                                                                            \
    code, [](StateMachineBuilder *self, const ReNode &node,                    \
             const BuildItem &state) {                                         \
      self->build_simple_node(node, state, type, label);                       \
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
        BUILD_MAP_ITEM(OpCode::COUNT, build_COUNT),
        BUILD_MAP_ITEM_SIMPLE(OpCode::WILDCARD,
                              TransitionFunctor::Type::WILDCARD, " =  . "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::DIGIT, TransitionFunctor::Type::DIGIT,
                              " =  \\d "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::WORD_CHAR,
                              TransitionFunctor::Type::WORD_CHAR, " =  \\w "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::NON_DIGIT,
                              TransitionFunctor::Type::NON_DIGIT, " =  \\D "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::NON_WORD_CHAR,
                              TransitionFunctor::Type::NON_WORD_CHAR,
                              " =  \\W "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::WHITESPACE,
                              TransitionFunctor::Type::WHITESPACE, " =  \\s "),
        BUILD_MAP_ITEM_SIMPLE(OpCode::NON_WHITESPACE,
                              TransitionFunctor::Type::NON_WHITESPACE,
                              " =  \\S "),
        BUILD_MAP_THROW(OpCode::RANGE),

};

void StateMachineBuilder::build_Node(const ReNode &current_node,
                                     const BuildItem &build_state) {
  auto search = build_map.find(static_cast<OpCode>(current_node.content));
  if (search == build_map.end()) {
    build_equal_node(current_node, build_state),
        " = " + std::string(1, static_cast<char>(current_node.content));
  } else {
    search->second(this, current_node, build_state);
  }
}

StateMachine StateMachineBuilder::build() {

  state_machine.states.emplace_back(0);
  // state_machine.states.emplace_back(0);
  state_machine.start_state = 0;
  state_machine.final_state = 0;

  tree_deque.emplace_back(tree->start_node);

  while (!tree_deque.empty()) {

    prev_node_id = state_machine.size() - 1;

    const BuildItem build_state = tree_deque.back();
    tree_deque.pop_back();

    const ReNode &current_node = tree->getNode(build_state.tree_node);

    // size_t next_node_id;

    // next_node_id = state_machine.size();

    DEBUG_STDOUT((build_state.entering ? "Enter" : "Exit")
                 << " state: "
                 << static_cast<regex::OpCode>(current_node.content)
                 << '\n'); // " from "
    //  << prev_node_id << " to  " << next_node_id << '\n');

    build_Node(current_node, build_state);

    // if (next_node_id != state_machine.size())
  }
  state_machine.final_state = state_machine.size() - 1;
  // state_machine.at(state_machine.final_state).default_transition =
  //     state_machine.final_state;

  return state_machine;
}

void StateMachineBuilder::build_ROOT(const ReNode &current_node,
                                     const BuildItem &build_state) {
#ifdef DEBUG
  if (!build_state.entering)
    throw std::runtime_error("Root node exit.");
#else
  (void)build_state;
#endif

  for (auto it = current_node.children.rbegin();
       it != current_node.children.rend(); ++it) {
    tree_deque.emplace_back(*it);
  }
}

void StateMachineBuilder::build_BRACKET(const ReNode &current_node,
                                        const BuildItem & /*build_state*/) {
  state_machine.states.emplace_back(0);
  for (unsigned child_id : current_node.children) {
    const ReNode &child_node = tree->getNode(child_id);
    if (child_node.content == OpCode::RANGE) {
      auto r_min = tree->getNode(child_node.children[0]).content;
      auto r_max = tree->getNode(child_node.children[1]).content;

      state_machine.at(prev_node_id)
          .pushTransition(state_machine.size() - 1, TransitionFunctor::RANGE,
                          r_min, r_max);

      state_machine.at(prev_node_id)
          .pushTransitionLabel(std::string(1, static_cast<char>(r_min)) +
                               " - " +
                               std::string(1, static_cast<char>(r_max)));
    } else {
      state_machine.at(prev_node_id)
          .pushTransition(state_machine.size() - 1, TransitionFunctor::EQUAL,
                          child_node.content);
      state_machine.at(prev_node_id)
          .pushTransitionLabel(
              " = " + std::string(1, static_cast<char>(child_node.content)));
    }
  }
}

void StateMachineBuilder::build_REPETITION(const ReNode &current_node,
                                           const BuildItem &build_state) {
  if (build_state.entering) {

    tree_deque.emplace_back(build_state.tree_node, state_machine.size() - 1,
                            false);

    for (auto riter = current_node.children.rbegin();
         riter != current_node.children.rend(); ++riter) {
      tree_deque.emplace_back(*riter);
    }

  } else {

    StateNode &loop_node = state_machine.states.at(state_machine.size() - 1);
    loop_node.push_E_transition(build_state.state_machine_node);
    loop_node.push_E_transition(state_machine.size());
    state_machine.states.emplace_back(0);
  }
}

void StateMachineBuilder::build_KLEENE_STAR(const ReNode &current_node,
                                            const BuildItem &build_state) {
  if (build_state.entering) {

    state_machine.states.back().push_E_transition(state_machine.size());
    state_machine.states.emplace_back(0);

    // emplace_back KLEENE_STAR as exit node to close loop
    tree_deque.emplace_back(build_state.tree_node, state_machine.size() - 1,
                            false);

    for (auto riter = current_node.children.rbegin();
         riter != current_node.children.rend(); ++riter) {
      tree_deque.emplace_back(*riter);
    }

  } else {

    state_machine.states.emplace_back(0);

    StateNode &loop_node = state_machine.states.at(prev_node_id);

    DEBUG_STDERR("push_E_transition from : "
                 << prev_node_id << " to " << state_machine.size() - 1 << '\n');
    loop_node.push_E_transition(state_machine.size() - 1);

    loop_node.push_E_transition(build_state.state_machine_node);

    state_machine.states.at(build_state.state_machine_node - 1)
        .push_E_transition(state_machine.size() - 1);
  }
}

// void StateMachineBuilder::build_RANGE(const ReNode &current_node,
//                                       const BuildItem &build_state) {}

void StateMachineBuilder::build_OPTIONAL(const ReNode &current_node,
                                         const BuildItem &build_state) {
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

void StateMachineBuilder::build_COUNT(const ReNode &current_node,
                                      const BuildItem &build_state) {
  if (build_state.entering) {
    std::string low_count;
    std::string upper_count;

    size_t i = 0;
    while (++i < current_node.children.size() &&
           tree->getNode(current_node.children.at(i)).content != ',') {
      low_count +=
          static_cast<char>(tree->getNode(current_node.children.at(i)).content);
    }
    while (++i < current_node.children.size()) {
      upper_count +=
          static_cast<char>(tree->getNode(current_node.children.at(i)).content);
    }

    if (upper_count.empty()) {
      size_t count = std::stoi(low_count);
      DEBUG_STDOUT(" count_string : " << low_count << '\n')
      DEBUG_STDOUT(" Adding count : " << count << '\n')
      for (size_t i = 0; i < count; i++) {
        tree_deque.emplace_back(current_node.children.at(0));
      }
    } else {
      size_t low = std::stoi(low_count);
      size_t high = std::stoi(upper_count);
      DEBUG_STDOUT(" count_string : " << low << '-' << high << '\n')
      for (size_t i = 0; i < low; i++) {
        tree_deque.emplace_back(current_node.children.at(0));
      }
      for (size_t i = low; i < high; i++) {
        tree_deque.emplace_back(current_node.children.at(0));
        tree_deque.emplace_back(build_state.tree_node, state_machine.size() - 1,
                                false);
      }
    }
  } else {

    // state_machine.states.at(build_state.state_machine_node)
    //     .push_E_transition(prev_node_id);
  }
}

void StateMachineBuilder::build_AT_START(const ReNode &current_node,
                                         const BuildItem & /*build_state*/) {
  // state_machine.states.emplace_back(0);
  // state_machine.start_state = 1;

  for (auto riter = current_node.children.rbegin();
       riter != current_node.children.rend(); ++riter) {
    tree_deque.emplace_back(*riter);
  }
}

void StateMachineBuilder::build_AT_END(const ReNode &current_node,
                                       const BuildItem &build_state) {
  if (build_state.entering) {

    for (auto riter = current_node.children.rbegin();
         riter != current_node.children.rend(); ++riter) {
      tree_deque.emplace_back(*riter);
    }
    tree_deque.emplace_back(build_state.tree_node, state_machine.size() - 1,
                            false);
  } else {
  }
}

void StateMachineBuilder::build_UNION(const ReNode &current_node,
                                      const BuildItem &build_state) {
  if (build_state.entering) {

    state_machine.states.emplace_back(0);
    // emplace_back UNION as exit node to close loop
    tree_deque.emplace_back(build_state.tree_node, state_machine.size() - 1,
                            false);

    for (auto riter = current_node.children.rbegin();
         riter != current_node.children.rend(); ++riter) {
      tree_deque.emplace_back(*riter, state_machine.size() - 1, true);
    }
  } else {
    state_machine.states.emplace_back(0);
    state_machine.states.at(build_state.state_machine_node)
        .push_E_transition(state_machine.size() - 1);
    // prev_node_id = build_state.state_machine_node;
  }
}

void StateMachineBuilder::build_UNION_SUBEXPRESION(
    const ReNode &current_node, const BuildItem &build_state) {
  if (build_state.entering) {

    state_machine.states.emplace_back(0);
    state_machine.states.at(build_state.state_machine_node - 1)
        .push_E_transition(state_machine.size() - 1);

    // emplace_back UNION as exit node to close loop
    tree_deque.emplace_back(build_state.tree_node,
                            build_state.state_machine_node, false);

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

void StateMachineBuilder::build_simple_node(const ReNode &current_node,
                                            const BuildItem &build_state,
                                            const TransitionFunctor::Type type,
                                            const std::string &label) {

  state_machine.states.emplace_back(0);
  DEBUG_STDOUT("Add Transition  = "
               << static_cast<regex::OpCode>(current_node.content) << " to "
               << state_machine.size() - 1 << '\n');
  state_machine.at(prev_node_id).pushTransition(state_machine.size() - 1, type);
  state_machine.at(prev_node_id).pushTransitionLabel(label /* " =  \\s " */);
}

void StateMachineBuilder::build_equal_node(const ReNode &current_node,
                                           const BuildItem &build_state,
                                           const std::string &label) {

  state_machine.states.emplace_back(0);
  DEBUG_STDOUT("Add Transition  = "
               << static_cast<regex::OpCode>(current_node.content) << " to "
               << state_machine.size() - 1 << '\n');
  state_machine.at(prev_node_id)
      .pushTransition(state_machine.size() - 1, TransitionFunctor::Type::EQUAL,
                      current_node.content);
  state_machine.at(prev_node_id).pushTransitionLabel(label /* " =  \\s " */);
}

StateMachine buildStateMachineFromTree(const ReTree &tree) {
  StateMachineBuilder builder(&tree);
  return builder.build();
}

} // namespace regex