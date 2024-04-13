Status GraphConstructor::IsNodeFullyMapped(const NodeDef& node_def,
                                           bool* is_node_mapped) {
  const OpDef* op_def;
  TF_RETURN_IF_ERROR(g_->op_registry()->LookUpOpDef(node_def.op(), &op_def));
  for (int i = 0; i < op_def->output_arg_size(); ++i) {
    if (opts_.input_map.find({node_def.name(), i}) == opts_.input_map.end()) {
      *is_node_mapped = false;
      return Status::OK();
    }
  }
  *is_node_mapped = true;
  return Status::OK();
}