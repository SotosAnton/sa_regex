#include <iostream>
#include <regex/state_machine.hpp>
#include <stack>
#include <stdexcept>

namespace regex {

// size_t StateMachine::splitNodes(const size_t origin_id,
//                                 const size_t destination_id,
//                                 const size_t default_transition) {

//   StateNode &origin = states.at(origin_id);
//   StateNode &destination = states.at(destination_id);

//   states.emplace_back(default_transition);
//   size_t new_destination = states.size() - 1;

//   for (auto &transition : origin.transitions) {
//     if (transition.destination == destination_id)
//       transition.destination = new_destination;
//   }
// }

bool runStateMachine(const StateMachine &engine, const std::string &input) {

  // MachineState state(engine.start_state, 0);
  std::stack<MachineState> exec_stack;
  exec_stack.emplace(MachineState(engine.start_state, 0));

  DEBUG_STDOUT("Start run from: " << engine.start_state
                                  << " Final: " << engine.final_state << '\n');
  DEBUG_STDOUT(" Input:" << input << '\n')

#ifdef DEBUG
  unsigned step_counter = 0;
#endif

  MachineState state;

  while (!exec_stack.empty()) {

    state = exec_stack.top();
    exec_stack.pop();

    DEBUG_STDOUT("State: " << state.node_id)
#ifdef DEBUG
    step_counter++;
#endif
    auto &node = engine.at(state.node_id);

    bool transisition_success = false;

    if (state.input_id < input.size()) {

      char c = input[state.input_id];

      DEBUG_STDOUT(" Input : " << state.input_id << " c: " << c << '\n')

      // node.state = state.input_id;
      for (auto transition : node.transitions) {
        if (transition.func(c)) {
          exec_stack.emplace(transition.destination, state.input_id + 1);
          transisition_success = true;
          DEBUG_STDOUT(" Valid = " << transition.destination << " "
                                   << state.input_id + 1 << '\n');
        }
      }
    }

    for (auto transition : node.e_transitions) {
      DEBUG_STDOUT(" push e = " << transition << " " << state.input_id << '\n');
      exec_stack.emplace(transition, state.input_id);
      transisition_success = true;
    }

    if (state.node_id ==
            engine.final_state /*&& state.input_id == input.size()*/
        && state.input_id > 0) {
      DEBUG_STDERR("Reached final state : " << engine.final_state << " at "
                                            << state.input_id << '\n');
      DEBUG_STDERR("step_counter : " << step_counter << '\n');
      return true;
    }
  }

  DEBUG_STDOUT(" exec_stack : " << exec_stack.size() << "\n")

  return false;
}

void depthFirstSearch(const MachineState &start_state,
                      std::unique_ptr<StateContainer> &state_container,
                      const StateMachine &engine) {
  std::stack<size_t> node_stack;
  node_stack.emplace(start_state.node_id);
  while (!node_stack.empty()) {
    auto &node = engine.at(node_stack.top());
    node_stack.pop();
    for (auto &transision : node.e_transitions) {
      MachineState tmp_state(transision, start_state.input_id);
      if (!state_container->contains(tmp_state)) {
        state_container->insert(tmp_state);
        node_stack.push(transision);
      }
    }
  }
}

bool runStateMachineSmart(const StateMachine &engine,
                          const std::string &input) {

  // MachineState state(engine.start_state, 0);
  std::unique_ptr<StateContainer> current_stateSet =
      std::make_unique<StateContainer>();
  std::unique_ptr<StateContainer> next_stateSet =
      std::make_unique<StateContainer>();

#ifdef DEBUG
  unsigned step_counter = 0;
#endif

  current_stateSet->emplace(MachineState(engine.start_state, 0));
  depthFirstSearch(MachineState(engine.start_state, 0), current_stateSet,
                   engine);

  DEBUG_STDOUT("Start run from: " << engine.start_state
                                  << " Final: " << engine.final_state << '\n');
  DEBUG_STDOUT(" Input:" << input << '\n')

  while (!current_stateSet->empty()) {

    for (auto it = current_stateSet->begin(); it != current_stateSet->end();
         it++) {
      const MachineState &state = *it;

      DEBUG_STDOUT("State: " << state.node_id)
#ifdef DEBUG
      step_counter++;
#endif
      auto &node = engine.at(state.node_id);

      if (state.input_id < input.size()) {

        char c = input[state.input_id];

        DEBUG_STDOUT(" Input : " << state.input_id << " c: " << c << '\n')

        // node.state = state.input_id;
        for (auto transition : node.transitions) {
          if (transition.func(c)) {
            next_stateSet->emplace(transition.destination, state.input_id + 1);
            depthFirstSearch(
                MachineState(transition.destination, state.input_id + 1),
                next_stateSet, engine);
            DEBUG_STDOUT(" Valid = " << transition.destination << " "
                                     << state.input_id + 1 << '\n');
          }
        }
      }

      if (state.node_id ==
              engine.final_state /*&& state.input_id == input.size()*/
          && state.input_id > 0) {
        DEBUG_STDERR("Reached final state : " << engine.final_state << " at "
                                              << state.input_id << '\n');
        DEBUG_STDERR("step_counter : " << step_counter << '\n');

        return true;
      }
    }

    std::swap(current_stateSet, next_stateSet);
    next_stateSet->clear();
  }

  DEBUG_STDOUT(" current_stateSet : " << current_stateSet->size() << "\n")
  DEBUG_STDOUT(" next_stateSet : " << next_stateSet->size() << "\n")
  DEBUG_STDERR("step_counter : " << step_counter << '\n');

  return false;
}

} // namespace regex