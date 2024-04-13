const FunctionDef* FunctionLibraryDefinition::Find(const string& func) const {
  tf_shared_lock l(mu_);
  auto result = FindHelper(func);
  if (result) {
    return &result->fdef;
  } else {
    return nullptr;
  }
}