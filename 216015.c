Status ConvertNodeDefsToGraph(const GraphConstructorOptions& opts,
                              gtl::ArraySlice<NodeDef> nodes, Graph* g) {
  ShapeRefiner refiner(TF_GRAPH_DEF_VERSION, g->op_registry());
  // TODO(irving): Copy will go away once NodeInfo exists
  std::vector<const NodeDef*> node_defs;
  node_defs.reserve(nodes.size());
  for (const auto& n : nodes) {
    node_defs.push_back(&n);
  }
  return GraphConstructor::Construct(opts, node_defs, nullptr, nullptr, g,
                                     &refiner, /*return_tensors=*/nullptr,
                                     /*return_nodes=*/nullptr,
                                     /*missing_unused_input_map_keys=*/nullptr);
}