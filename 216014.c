  const NodeDef& get_node_def(int i) const override {
    CHECK(!is_consumed_[i])
        << "NodeDef " << i << " accessed after it was consumed.";
    return graph_def_.node(i);
  }