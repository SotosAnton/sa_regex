#pragma once
#include "functional"
#include "regex/reTree.hpp"
#include "regex/state_machine.hpp"
#include "regex/util.hpp"

namespace regex {

StateMachine buildStateMachineFromTree(const ReTree &tree);

class StateMachineBuilder {

public:
  StateMachineBuilder(const ReTree *tree) : tree(tree) {}
  StateMachine build();

  struct BuildItem {
    size_t tree_node;          // tree node
    size_t state_machine_node; // state id that was added when entering node
    // bool loop_back;
    bool entering; // visit node top->down or bottom->up

    BuildItem(size_t a) : tree_node(a), state_machine_node(0), entering(true) {}

    BuildItem(size_t tree_node, size_t state_machine_node, bool entering)
        : tree_node(tree_node), state_machine_node(state_machine_node),
          entering(entering) {}
  };

private:
  void nodeEntry(const ReNode &current_node, const BuildItem &build_state,
                 size_t &prev_node_id, size_t &next_node_id);

  void nodeExit(const ReNode &current_node, const BuildItem &build_state,
                size_t &prev_node_id, size_t &next_node_id);

  StateMachine state_machine;
  const ReTree *tree;
  std::deque<BuildItem> tree_deque;

  void build_ROOT(const ReNode &current_node, const BuildItem &build_state,
                  size_t &prev_node_id, size_t &next_node_id);

  void build_BRACKET(const ReNode &current_node, const BuildItem &build_state,
                     size_t &prev_node_id, size_t &next_node_id);

  void build_KLEENE_STAR(const ReNode &current_node,
                         const BuildItem &build_state, size_t &prev_node_id,
                         size_t &next_node_id);

  void build_REPETITION(const ReNode &current_node,
                        const BuildItem &build_state, size_t &prev_node_id,
                        size_t &next_node_id);

  void build_RANGE(const ReNode &current_node, const BuildItem &build_state,
                   size_t &prev_node_id, size_t &next_node_id);

  void build_OPTIONAL(const ReNode &current_node, const BuildItem &build_state,
                      size_t &prev_node_id, size_t &next_node_id);

  void build_AT_START(const ReNode &current_node, const BuildItem &build_state,
                      size_t &prev_node_id, size_t &next_node_id);

  void build_simple_node(const ReNode &current_node,
                         const BuildItem &build_state, size_t &prev_node_id,
                         size_t &next_node_id,
                         const regex::TransisionFunction &func,
                         const std::string &label = "");
};

} // namespace regex
