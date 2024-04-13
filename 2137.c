Status FunctionLibraryDefinition::RemoveFunctionHelper(const string& func) {
  const auto& i = function_defs_.find(func);
  if (i == function_defs_.end()) {
    return errors::InvalidArgument("Tried to remove non-existent function '",
                                   func, "'.");
  }
  function_defs_.erase(i);
  return Status::OK();
}