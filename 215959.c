Status GraphConstructor::ModifyNodeDefForImport(NodeDef* node_def) {
  const OpDef* op_def;
  TF_RETURN_IF_ERROR(g_->op_registry()->LookUpOpDef(node_def->op(), &op_def));
  AddDefaultsToNodeDef(*op_def, node_def);
  TF_RETURN_IF_ERROR(ValidateNodeDef(*node_def, *op_def));
  if (versions()) {
    TF_RETURN_IF_ERROR(CheckOpDeprecation(*op_def, versions()->producer()));
  }
  return Status::OK();
}