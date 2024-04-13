void GraphConstructor::UpdateUniquifiedColocationNames() {
  for (const auto& pair : gdef_nodes_) {
    Node* node = pair.second.node;
    if (node == nullptr) continue;
    std::vector<string> coloc_values;
    if (!TryGetNodeAttr(node->attrs(), kColocationAttrName, &coloc_values))
      continue;
    bool updated = false;
    for (size_t i = 0; i < coloc_values.size(); ++i) {
      StringPiece val(coloc_values[i]);
      if (absl::ConsumePrefix(&val, kColocationGroupPrefix)) {
        auto name_pair = uniquified_names_.find(string(val));
        if (name_pair == uniquified_names_.end()) continue;
        updated = true;
        coloc_values[i] =
            strings::StrCat(kColocationGroupPrefix, name_pair->second);
      }
    }
    if (updated) {
      node->AddAttr(kColocationAttrName, std::move(coloc_values));
    }
  }
}