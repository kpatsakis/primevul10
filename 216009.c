void GraphConstructor::UniquifyNames(
    const std::vector<bool>& input_already_exists, NodeDef* node_def) {
  if (NameExistsInGraph(node_def->name())) {
    string old_name = node_def->name();
    node_def->set_name(FindUniqueName(node_def->name()));
    uniquified_names_[old_name] = node_def->name();
    // Note that we don't have to update gdef_nodes_ or gdef_prefixes_ with
    // `name` because we guarantee the original NodeDef names are unique,
    // meaning we won't generate this name again.
  }
  for (int i = 0; i < node_def->input_size(); ++i) {
    // Skip remapped inputs (which already exist in g_ and are not being
    // imported).
    if (input_already_exists[i]) continue;
    TensorId id = ParseTensorName(node_def->input(i));
    // We require that UniquifyNames() is called on all NodeDefs in topological
    // order. This guarantees that node_def's inputs will already be uniquified
    // if necessary.
    auto iter = uniquified_names_.find(string(id.first));
    if (iter == uniquified_names_.end()) continue;
    id.first = iter->second;
    node_def->set_input(i, id.ToString());
  }
}