  NodeDef consume_node_def(int i) override {
    CHECK(!is_consumed_[i]) << "NodeDef " << i << " consumed twice.";
    is_consumed_[i] = true;
    return std::move(*graph_def_.mutable_node(i));
  }