inline bool IsMerge(const NodeDef& node_def) {
  return node_def.op() == "Merge" || node_def.op() == "RefMerge" ||
         node_def.op() == "_XlaMerge";
}