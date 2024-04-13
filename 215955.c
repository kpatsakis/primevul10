/* static */ Status GraphConstructor::Construct(
    const Options& opts, NodeDefSlice node_defs, const VersionDef* versions,
    const FunctionDefLibrary* library, Graph* g, ShapeRefiner* refiner,
    std::vector<std::pair<Node*, int>>* return_tensors,
    std::vector<Node*>* return_nodes,
    std::vector<SafeTensorId>* missing_unused_input_map_keys) {
  if (versions) {
    TF_RETURN_IF_ERROR(CheckVersions(*versions, TF_GRAPH_DEF_VERSION,
                                     TF_GRAPH_DEF_VERSION_MIN_PRODUCER,
                                     "GraphDef", "graph"));
  }
  NodeDefCopyingGraphConstructor c(opts, node_defs, versions, library, g,
                                   refiner, return_tensors, return_nodes,
                                   missing_unused_input_map_keys);
  Status s = c.TryImport();
  if (!s.ok()) {
    c.Undo();
    s = MaybeAppendVersionWarning(versions, s);
  }
  return s;
}