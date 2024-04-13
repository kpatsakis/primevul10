Status FunctionLibraryDefinition::Remove(
    const std::vector<string>& funcs,
    const std::vector<string>& funcs_with_grads) {
  Status s;
  for (const string& f : funcs) {
    s = RemoveFunctionHelper(f);
    if (!s.ok()) {
      return s;
    }
  }
  for (const string& f : funcs_with_grads) {
    s = RemoveGradient(f);
    if (!s.ok()) {
      return s;
    }
  }
  return Status::OK();
}