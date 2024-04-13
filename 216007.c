/* static */ Status GraphConstructor::Construct(
    const Options& opts, GraphDef&& graph_def, Graph* g, ShapeRefiner* refiner,
    std::vector<std::pair<Node*, int>>* return_tensors,
    std::vector<Node*>* return_nodes,
    std::vector<SafeTensorId>* missing_unused_input_map_keys) {
  TF_RETURN_IF_ERROR(CheckVersions(graph_def.versions(), TF_GRAPH_DEF_VERSION,
                                   TF_GRAPH_DEF_VERSION_MIN_PRODUCER,
                                   "GraphDef", "graph"));
  VersionDef version_def = graph_def.versions();
  NodeDefMovingGraphConstructor c(opts, std::move(graph_def), g, refiner,
                                  return_tensors, return_nodes,
                                  missing_unused_input_map_keys);
  Status s = c.TryImport();
  if (!s.ok()) {
    c.Undo();
    s = MaybeAppendVersionWarning(&version_def, s);
  }
  return s;
}