  Status TryImport() {
    TF_RETURN_IF_ERROR(EnsureNoNameCollisions());
    TF_RETURN_IF_ERROR(ValidateInputMapAndControlDependencies());
    TF_RETURN_IF_ERROR(BuildNodeIndex());
    TF_RETURN_IF_ERROR(InitFromEdges());

    // NOTE: Convert() invokes `consume_node_def()` on each node in the input
    // graph, so `get_node_def()` is no longer usable once it is called.
    TF_RETURN_IF_ERROR(Convert());

    TF_RETURN_IF_ERROR(AddBackEdges());
    TF_RETURN_IF_ERROR(UpdateVersionDef());
    TF_RETURN_IF_ERROR(PopulateReturnTensors());
    TF_RETURN_IF_ERROR(PopulateReturnNodes());
    TF_RETURN_IF_ERROR(PopulateMissingUnusedInputMapKeys());
    UpdateUniquifiedColocationNames();
    FixupSourceAndSinkEdges(g_);
    return Status::OK();
  }