Status ConvertGraphDefToGraph(const GraphConstructorOptions& opts,
                              GraphDef&& gdef, Graph* g) {
  ShapeRefiner refiner(gdef.versions().producer(), g->op_registry());
  return GraphConstructor::Construct(opts, std::move(gdef), g, &refiner,
                                     /*return_tensors=*/nullptr,
                                     /*return_nodes=*/nullptr,
                                     /*missing_unused_input_map_keys=*/nullptr);
}