Status GraphConstructor::InitFromEdges() {
  const int num_nodes = node_def_count();
  pending_count_.reserve(num_nodes);
  outputs_.resize(num_nodes);
  gtl::FlatSet<string> next_iteration_nodes;
  for (int n = 0; n < node_def_count(); ++n) {
    const NodeDef& node_def = get_node_def(n);
    if (IsNextIteration(node_def)) {
      next_iteration_nodes.insert(node_def.name());
    }
  }

  // Parse the inputs for each node.
  for (int n = 0; n < num_nodes; ++n) {
    const NodeDef& node_def = get_node_def(n);
    int pending_count = node_def.input_size();
    if (IsMerge(node_def)) {
      // Cycles in the graph are only allowed for while loops. A while loop is
      // identified by an edge from a NextIteration node to a Merge node. For
      // such Merge nodes, only wait for one non-control input before
      // considering the node ready to process in Convert().
      int32 num_control_edges = 0;
      bool has_loop_back_edge = false;
      for (int i = 0; i < node_def.input_size(); ++i) {
        StringPiece input_name(node_def.input(i));
        if (absl::StartsWith(input_name, "^")) {
          num_control_edges++;
        } else {
          TensorId id(ParseTensorName(input_name));
          if (next_iteration_nodes.find(string(id.first)) !=
              next_iteration_nodes.end()) {
            has_loop_back_edge = true;
          }
        }
      }
      if (has_loop_back_edge) {
        pending_count = num_control_edges + 1;
      }
    }
    for (int i = 0; i < node_def.input_size(); ++i) {
      StringPiece input_name = node_def.input(i);
      TensorId id(ParseTensorName(input_name));
      if (opts_.input_map.count(id) == 0) {
        // If an input is not mapped, then the input should appear in the graph
        // being imported.
        auto iter = gdef_nodes_.find(id.first);
        if (iter == gdef_nodes_.end()) {
          return errors::InvalidArgument("Node '", node_def.name(),
                                         "': Unknown input node '",
                                         node_def.input(i), "'");
        }
        outputs_[iter->second.gdef_index].push_back(n);
      } else {
        // This input is mapped to an existing edge. Therefore this input is
        // as good as being already processed.
        --pending_count;
        DCHECK_GE(pending_count, 0);
      }
    }
    if (pending_count == 0) {
      ready_.insert(n);
    }
    pending_count_.push_back(pending_count);
  }
  return Status::OK();
}