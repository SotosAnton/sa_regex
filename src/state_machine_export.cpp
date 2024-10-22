#pragma once
#include <iostream>
#include <regex/state_machine_export.hpp>

namespace regex {

using std::to_string;

std::string generateStateMachineDescription(const StateMachine &engine) {

  std::ostringstream output;
  for (size_t i = 0; i < engine.size(); i++) {
    const StateNode &current_node = engine.states.at(i);

    if (i == engine.start_state)
      output << "s:" << to_string(i) << '\n';
    if (i == engine.final_state)
      output << "f:" << to_string(i) << '\n';

    for (auto &transision : current_node.transisions) {

      output << to_string(i) << " -> " << to_string(transision.destination);
      if (!transision.label.empty())
        output << " [" << transision.label << "]";
      output << '\n';
    }

    if (current_node.e_transision.active()) {
      output << to_string(i) << " -> "
             << to_string(current_node.e_transision.get()) << " [e]" << '\n';
    }

    if (current_node.default_transision != engine.start_state) {
      output << to_string(i) << " -> "
             << to_string(current_node.default_transision) << " ["
             << " d "
             << "]";
      output << '\n';
    }
  }

  return output.str();
}

bool writeMachineDescriptionToFile(const std::string &file,
                                   const StateMachine &engine) {
  std::ofstream data_file;
  data_file.open(file, std::ios::out | std::ios::trunc);
  if (data_file.fail()) // if file is not avaliable
    return false;
  data_file << generateStateMachineDescription(engine);
  data_file.close();

  return true;
}

} // namespace regex