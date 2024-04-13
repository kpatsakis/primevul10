FunctionLibraryDefinition::FindHelper(const string& func) const {
  auto iter = function_defs_.find(func);
  if (iter == function_defs_.end()) {
    return nullptr;
  } else {
    return iter->second;
  }
}