  void AddDep(int node_index, int dep_index) {
    CHECK_LT(node_index, nodes_.size());
    nodes_[node_index].control_inputs.push_back(dep_index);
  }