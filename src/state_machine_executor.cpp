#include <iostream>
#include <regex/state_machine_executor.hpp>
#include <stack>
#include <stdexcept>

namespace regex {

bool StateMachineExecutor::search(
    const std::string input, std::vector<std::pair<size_t, size_t>> *matches) {
  if (!engine)
    throw std::runtime_error("StateMachineExecutor not initialized.");
  size_t start = 0;
  size_t match_index = 0;
  reset();

  while (start < input.size()) {
    if (runStateMachineSmart(input, start, &match_index)) {
      if (matches)
        matches->push_back({start, match_index - start});
      return true;
    }
    start++;
  }
  return false;
};

void StateMachineExecutor::reset() {
  set_1.clear();
  set_2.clear();
}
bool StateMachineExecutor::scan(
    const std::string input, std::vector<std::pair<size_t, size_t>> *matches) {
  if (!engine)
    throw std::runtime_error("StateMachineExecutor not initialized.");
  size_t start = 0;
  size_t match_index = 0;
  bool res = false;
  reset();

  while (start < input.size()) {
    if (runStateMachineSmart(input, start, &match_index)) {
      res = true;
      if (matches)
        matches->push_back({start, match_index - start});
      start = match_index;
    }
    start++;
  }
  return res;
};

bool StateMachineExecutor::match(const std::string input) {
  if (!engine)
    throw std::runtime_error("StateMachineExecutor not initialized.");

  reset();
  return runStateMachineSmart(input, 0);
};

void StateMachineExecutor::depthFirstSearch(const MachineState &start_state,
                                            StateContainer *state_container) {

  if (state_set.at(start_state.node_id).empty()) {
    std::stack<size_t> node_stack;
    node_stack.emplace(start_state.node_id);
    while (!node_stack.empty()) {
      auto &node = engine->at(node_stack.top());
      node_stack.pop();
      for (auto &transision : node.e_transitions) {
        MachineState tmp_state(transision, start_state.input_id);
        if (!state_container->contains(tmp_state)) {
          state_container->insert(tmp_state);
          node_stack.push(transision);
          state_set.at(start_state.node_id).push_back(transision);
        }
      }
    }
  } else {
    for (auto &item : state_set.at(start_state.node_id)) {
      MachineState tmp_state(item, start_state.input_id);
      if (!state_container->contains(tmp_state)) {
        state_container->insert(tmp_state);
      }
    }
  }
}

bool StateMachineExecutor::runStateMachineSmart(const std::string &input,
                                                const size_t start_index,
                                                size_t *end_index) {

  MachineState state(engine->start_state, 0);
  StateContainer *current_stateSet = &set_1;
  StateContainer *next_stateSet = &set_2;

#ifdef DEBUG
  unsigned step_counter = 0;
#endif

  current_stateSet->emplace(MachineState(engine->start_state, start_index));
  depthFirstSearch(MachineState(engine->start_state, start_index),
                   current_stateSet);

  DEBUG_STDOUT("Start run from: " << engine->start_state
                                  << " Final: " << engine->final_state << '\n');
  DEBUG_STDOUT(" Input:" << input << '\n')

  while (!current_stateSet->empty()) {

    for (auto it = current_stateSet->begin(); it != current_stateSet->end();
         it++) {
      const MachineState &state = *it;

      DEBUG_STDOUT("State: " << state.node_id)
#ifdef DEBUG
      step_counter++;
#endif
      auto &node = engine->at(state.node_id);

      if (state.input_id < input.size()) {

        char c = input[state.input_id];

        DEBUG_STDOUT(" Input : " << state.input_id << " c: " << c << '\n')

        // node.state = state.input_id;
        for (auto transition : node.transitions) {
          if (transition.func(c)) {
            next_stateSet->emplace(transition.destination, state.input_id + 1);
            depthFirstSearch(
                MachineState(transition.destination, state.input_id + 1),
                next_stateSet);
            DEBUG_STDOUT(" Valid = " << transition.destination << " "
                                     << state.input_id + 1 << '\n');
          }
        }
      }

      if (state.node_id ==
              engine->final_state /*&& state.input_id == input.size()*/
          && state.input_id > start_index) {
        DEBUG_STDERR("Reached final state : " << engine->final_state << " at "
                                              << state.input_id << '\n');
        DEBUG_STDERR("step_counter : " << step_counter << '\n');
        if (end_index)
          *end_index = state.input_id;
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

bool StateMachineExecutor::runStateMachine(const std::string &input,
                                           const size_t start_index) {

  // MachineState state(engine->start_state, 0);
  std::stack<MachineState> exec_stack;
  exec_stack.emplace(MachineState(engine->start_state, start_index));

  DEBUG_STDOUT("Start run from: " << engine->start_state
                                  << " Final: " << engine->final_state << '\n');
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
    auto &node = engine->at(state.node_id);

    if (state.input_id < input.size()) {

      char c = input[state.input_id];

      DEBUG_STDOUT(" Input : " << state.input_id << " c: " << c << '\n')

      // node.state = state.input_id;
      for (auto transition : node.transitions) {
        if (transition.func(c)) {
          exec_stack.emplace(transition.destination, state.input_id + 1);
          DEBUG_STDOUT(" Valid = " << transition.destination << " "
                                   << state.input_id + 1 << '\n');
        }
      }
    }

    for (auto transition : node.e_transitions) {
      DEBUG_STDOUT(" push e = " << transition << " " << state.input_id << '\n');
      exec_stack.emplace(transition, state.input_id);
    }

    if (state.node_id ==
            engine->final_state /*&& state.input_id == input.size()*/
        && state.input_id > 0) {
      DEBUG_STDERR("Reached final state : " << engine->final_state << " at "
                                            << state.input_id << '\n');
      DEBUG_STDERR("step_counter : " << step_counter << '\n');
      return true;
    }
  }

  DEBUG_STDOUT(" exec_stack : " << exec_stack.size() << "\n")

  return false;
}

} // namespace regex