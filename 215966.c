void GraphConstructor::AddPrefixToNodeDef(
    const std::vector<bool>& input_already_exists, NodeDef* node_def) {
  if (prefix_.empty()) return;
  node_def->set_name(strings::StrCat(prefix_, node_def->name()));
  // Update names of input nodes
  for (int i = 0; i < node_def->input_size(); ++i) {
    // Skip remapped inputs (which already exist in g_ and are not being
    // imported).
    if (input_already_exists[i]) continue;
    StringPiece input(node_def->input(i));
    if (absl::ConsumePrefix(&input, "^")) {
      node_def->set_input(i, strings::StrCat("^", prefix_, input));
    } else {
      node_def->set_input(i, strings::StrCat(prefix_, input));
    }
  }
  // Update names of colocation groups
  if (node_def->attr().find(kColocationAttrName) != node_def->attr().end()) {
    auto* list =
        node_def->mutable_attr()->at(kColocationAttrName).mutable_list();
    for (int i = 0; i < list->s_size(); ++i) {
      StringPiece v(list->s(i));
      if (absl::ConsumePrefix(&v, kColocationGroupPrefix)) {
        list->set_s(i, strings::StrCat(kColocationGroupPrefix, prefix_, v));
      }
    }
  }
}