FunctionLibraryDefinition FunctionLibraryDefinition::ReachableDefinitions(
    const GraphDef& graph) const {
  return ReachableFunctionLibraryDefinition(*this, graph.node());
}