Status FunctionLibraryDefinition::RemoveGradient(const string& func) {
  const auto& i = func_grad_.find(func);
  if (i == func_grad_.end()) {
    return errors::InvalidArgument("Tried to remove non-existent gradient '",
                                   func, "'.");
  }
  func_grad_.erase(i);
  return Status::OK();
}