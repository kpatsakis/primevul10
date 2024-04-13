  SubgraphGuard(TfLiteContext* context, bool* is_subgraph_in_use)
      : is_subgraph_in_use_(is_subgraph_in_use) {
    if (*is_subgraph_in_use_) {
      TF_LITE_KERNEL_LOG(
          context,
          "Subgraph is already in use. Using an interpreter or a subgraph in "
          "multiple threads is not supported. Recursion in the graph is not "
          "supported.");
      status_ = kTfLiteError;
    } else {
      *is_subgraph_in_use_ = true;
    }
  }