void RemoveInputs(const std::vector<int>& inputs_to_remove, NodeDef* node_def,
                  std::vector<bool>* input_already_exists) {
  // Remove 'inputs_to_remove' from 'node_def'
  NodeDef copy;
  copy.mutable_input()->Reserve(node_def->input_size() -
                                inputs_to_remove.size());
  for (int i = 0, j = 0; i < node_def->input_size(); ++i) {
    if (j < inputs_to_remove.size() && i == inputs_to_remove[j]) {
      ++j;
    } else {
      copy.add_input()->swap(*node_def->mutable_input(i));
    }
  }
  node_def->mutable_input()->Swap(copy.mutable_input());
  // Remove 'inputs_to_remove' from 'input_already_exists'
  for (int idx : inputs_to_remove) {
    input_already_exists->erase(input_already_exists->begin() + idx);
  }
  DCHECK_EQ(input_already_exists->size(), node_def->input_size());
}