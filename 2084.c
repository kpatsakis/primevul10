Status FunctionLibraryDefinition::GetAttr(const Node& node, const string& attr,
                                          T* value) const {
  return GetAttr(node.def(), attr, value);
}