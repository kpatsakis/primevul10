string FunctionLibraryDefinition::FindGradientHelper(const string& func) const {
  return gtl::FindWithDefault(func_grad_, func, "");
}