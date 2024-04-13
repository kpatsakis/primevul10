string DebugString(gtl::ArraySlice<NodeDef> instantiated_func_nodes) {
  std::vector<const NodeDef*> ptrs;
  for (const NodeDef& n : instantiated_func_nodes) {
    ptrs.push_back(&n);
  }
  return Print(ptrs);
}