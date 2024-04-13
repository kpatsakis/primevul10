void FunctionLibraryDefinition::Clear() {
  mutex_lock l(mu_);
  function_defs_.clear();
  func_grad_.clear();
}