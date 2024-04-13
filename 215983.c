Status GraphConstructor::PopulateReturnNodes() {
  if (opts_.return_nodes.empty()) return Status::OK();
  for (StringPiece name : opts_.return_nodes) {
    auto iter = gdef_nodes_.find(name);
    if (iter == gdef_nodes_.end()) {
      return errors::InvalidArgument("Requested return node '", name,
                                     "' not found in graph def");
    }
    return_nodes_->push_back(iter->second.node);
  }
  return Status::OK();
}