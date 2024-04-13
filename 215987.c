Status GraphConstructor::EnsureNoNameCollisions() {
  existing_nodes_.reserve(g_->num_nodes());
  // Populate existing_nodes_ and existing_prefixes_.
  for (Node* n : g_->nodes()) {
    bool already_exists = !existing_nodes_.insert({n->name(), n}).second;
    if (already_exists) {
      if (NodeNameInValues(opts_.input_map, n->name())) {
        return errors::InvalidArgument(
            "cannot resolve input_map because multiple nodes exist with name '",
            n->name(), "'");
      }
      if (NodeNameInValues(opts_.control_dependencies, n->name())) {
        return errors::InvalidArgument(
            "cannot resolve control_dependencies because multiple nodes exist "
            "with name '",
            n->name(), "'");
      }
    }
    AddPrefixes(n->name(), &existing_prefixes_);
  }
  if (prefix_.empty() && opts_.importing && !opts_.uniquify_names) {
    for (size_t i = 0; i < node_def_count(); ++i) {
      const string& name = get_node_def(i).name();
      if (NameExistsInGraph(name)) {
        return errors::InvalidArgument("Node name '", name,
                                       "' already exists in the Graph");
      }
    }
  } else if (!prefix_.empty()) {
    StringPiece prefix_no_slash(prefix_);
    prefix_no_slash.remove_suffix(1);
    if (!IsValidNodeName(prefix_no_slash, false)) {
      return errors::InvalidArgument("Imported node name prefix '", prefix_,
                                     "' would lead to invalid node names");
    }
    if (NameExistsInGraph(prefix_no_slash) && opts_.uniquify_prefix) {
      prefix_ = strings::StrCat(FindUniqueName(prefix_no_slash), "/");
    }
  }
  return Status::OK();
}