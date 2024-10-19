#pragma once
#include "functional"
#include "regex/reTree.hpp"
#include "regex/state_machine.hpp"
#include "regex/util.hpp"

namespace regex {

StateMachine buildStateMachineFromTree(const ReTree &tree);

} // namespace regex
