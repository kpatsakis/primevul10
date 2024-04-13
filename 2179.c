Status GraphProperties::UpdateShapes(
    SymbolicShapeRefiner* shape_refiner,
    const absl::flat_hash_map<const NodeDef*, const NodeDef*>& resource_handles,
    const NodeDef* n, bool* new_shapes) const {
  if (IsEnter(*n)) {
    // The Enter shape function always forwards an UnknownShape, so do the right
    // thing here.
    TF_RETURN_IF_ERROR(UpdateEnter(shape_refiner, n, new_shapes));
  } else if (IsMerge(*n)) {
    // Properly handle merge nodes.
    TF_RETURN_IF_ERROR(UpdateMerge(shape_refiner, n, new_shapes));
  } else if (IsEnqueue(*n)) {
    // Make sure the shapes of enqueued tensors are propagated to the queue
    // itself.
    TF_RETURN_IF_ERROR(
        UpdateEnqueue(n, resource_handles, shape_refiner, new_shapes));
  } else if (IsQueue(*n)) {
    // Set shapes and types of Queue ops, if needed.
    TF_RETURN_IF_ERROR(UpdateQueue(n, shape_refiner, new_shapes));
  } else {
    // Rely on regular TF shape refinement for all the other nodes.
    // UpdateNode calls UpdateFunction if a function node is detected.
    TF_RETURN_IF_ERROR(shape_refiner->UpdateNode(n, new_shapes));
  }

  return Status::OK();
}