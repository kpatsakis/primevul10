bool FunctionLibraryDefinition::Contains(const string& func) const {
  tf_shared_lock l(mu_);
  return function_defs_.find(func) != function_defs_.end();
}