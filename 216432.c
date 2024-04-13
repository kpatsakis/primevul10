  ~SubgraphGuard() {
    // If tht original status was OK, recover the boolean flag.
    if (status_ == kTfLiteOk) {
      *is_subgraph_in_use_ = false;
    }
  }