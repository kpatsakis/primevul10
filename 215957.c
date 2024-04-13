void GraphConstructor::UpdatePendingCountAndReady(int processed,
                                                  bool is_next_iteration) {
  for (size_t i = 0; i < outputs_[processed].size(); ++i) {
    const int output = outputs_[processed][i];
    // We didn't consider NextIteration->Merge edges when computing
    // pending_counts_ so we should not have to consider it here either.
    bool is_next_iteration_to_merge_edge =
        is_next_iteration && merge_node_indices_.count(output) == 1;
    if (!is_next_iteration_to_merge_edge) {
      int* current_pending_count = &pending_count_[output];
      CHECK_GT(*current_pending_count, 0);
      (*current_pending_count)--;
      if (*current_pending_count == 0) {
        ready_.insert(output);
      }
    }
  }
}