Status FunctionLibraryDefinition::LookUp(
    const string& op, const OpRegistrationData** op_reg_data) const {
  tf_shared_lock l(mu_);
  auto iter = function_defs_.find(op);
  if (iter != function_defs_.end()) {
    *op_reg_data = &iter->second->op_registration_data;
    return Status::OK();
  }
  return default_registry_->LookUp(op, op_reg_data);
}