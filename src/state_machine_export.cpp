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

    for (auto &transition : current_node.transitions) {

      output << to_string(i) << " -> " << to_string(transition.destination);
      if (!transition.label.empty())
        output << " [" << transition.label << "]";
      output << '\n';
    }

    for (auto &transition : current_node.e_transitions) {
      output << to_string(i) << " -> " << to_string(transition) << " [e]"
             << '\n';
    }

    if (current_node.default_transition != engine.start_state) {
      output << to_string(i) << " -> "
             << to_string(current_node.default_transition) << " ["
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