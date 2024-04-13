Status GraphProperties::PropagateShapes(
    SymbolicShapeRefiner* shape_refiner, TopoQueue* new_shapes,
    const absl::flat_hash_map<const NodeDef*, const NodeDef*>& resource_handles,
    int num_loops) const {
  // Limit the number of iterations to prevent infinite loops in the presence of
  // incorrect shape functions. The algorithm should converge in at most
  // num_nested_loops^2 * max_rank. We approximate max_rank with the constant 4.
  // The same applies to resources.
  VLOG(1) << "Propagating " << new_shapes->size() << " new shapes through "
          << num_loops << " loops and " << resource_handles.size()
          << " resources" << std::endl;

  const int64_t max_loop_length = item_.graph.node_size();
  const int64_t max_rank = 4;
  const int64_t max_loop_iterations =
      max_rank * max_loop_length * std::max<int64_t>(1, num_loops * num_loops);
  const int64_t num_queues = resource_handles.size();
  const int64_t max_resource_iterations = num_queues * num_queues * max_rank;

  int64_t num_resource_iterations = 0;
  do {
    int64_t num_loop_iterations = 0;
    while (!new_shapes->empty() &&
           num_loop_iterations++ < max_loop_iterations) {
      const NodeDef* n = new_shapes->pop();
      bool updated = false;
      TF_RETURN_IF_ERROR(
          UpdateShapes(shape_refiner, resource_handles, n, &updated));
      if (updated) {
        for (const auto& fanout : shape_refiner->graph().GetFanouts(
                 *n, /*include_controlled_nodes=*/false)) {
          new_shapes->push(fanout.node);
        }
        // Make sure the corresponding queue nodes are (re)processed.
        if (IsEnqueue(*n)) {
          auto it = resource_handles.find(n);
          if (it != resource_handles.end()) {
            new_shapes->push(it->second);
          }
        }
      }
    }
  } while (!new_shapes->empty() &&
           num_resource_iterations++ < max_resource_iterations);

  if (!new_shapes->empty()) {
    return errors::Internal("Shape inference failed to converge");
  }

  return Status::OK();
}