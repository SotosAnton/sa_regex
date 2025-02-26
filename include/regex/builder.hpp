#pragma once
#include "functional"
#include "regex/reTree.hpp"
#include "regex/state_machine.hpp"
#include "regex/util.hpp"

namespace regex {

StateMachine buildStateMachineFromTree(const ReTree &tree);

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

class StateMachineBuilder {

  using BuildFunction = std::function<void(StateMachineBuilder *,
                                           const ReNode &, const BuildItem &)>;

public:
  StateMachineBuilder(const ReTree *tree) : tree(tree){};
  StateMachine build();

  const static std::unordered_map<OpCode, BuildFunction> build_map;

private:
  void build_Node(const ReNode &current_node, const BuildItem &build_state);

  StateMachine state_machine;
  const ReTree *tree;
  std::deque<BuildItem> tree_deque;
  size_t prev_node_id;

  void build_ROOT(const ReNode &current_node, const BuildItem &build_state);

  void build_BRACKET(const ReNode &current_node, const BuildItem &build_state);

  void build_KLEENE_STAR(const ReNode &current_node,
                         const BuildItem &build_state);

  void build_REPETITION(const ReNode &current_node,
                        const BuildItem &build_state);

  void build_OPTIONAL(const ReNode &current_node, const BuildItem &build_state);

  void build_AT_START(const ReNode &current_node, const BuildItem &build_state);

  void build_AT_END(const ReNode &current_node, const BuildItem &build_state);

  void build_UNION(const ReNode &current_node, const BuildItem &build_state);

  void build_COUNT(const ReNode &current_node, const BuildItem &build_state);

  void build_UNION_SUBEXPRESION(const ReNode &current_node,
                                const BuildItem &build_state);

  void build_simple_node(const ReNode &current_node,
                         const BuildItem &build_state,
                         const TransitionFunctor::Type type,
                         const std::string &label = "");

  void build_equal_node(const ReNode &current_node,
                        const BuildItem &build_state,
                        const std::string &label = "");
};

} // namespace regex
