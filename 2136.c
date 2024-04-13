string FunctionLibraryDefinition::FindGradient(const string& func) const {
  tf_shared_lock l(mu_);
  return gtl::FindWithDefault(func_grad_, func, "");
}