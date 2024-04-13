FunctionLibraryDefinition FunctionLibraryDefinition::ReachableDefinitions(
    const FunctionDef& func) const {
  return ReachableFunctionLibraryDefinition(*this, func.node_def());
}