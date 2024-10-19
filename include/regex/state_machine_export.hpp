#pragma once
#include <regex/state_machine.hpp>

namespace regex {

std::string generateStateMachineDescription(const StateMachine &engine);

bool writeMachineDescriptionToFile(const std::string &file,
                                   const StateMachine &engine);
} // namespace regex