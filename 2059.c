Status FunctionLibraryDefinition::CopyFunctionDefFrom(
    const string& func, const FunctionLibraryDefinition& other) {
  if (default_registry_ != other.default_registry_) {
    return errors::InvalidArgument(
        "Cannot copy function '", func,
        "' because CopyFunctionDefFrom() requires that both libraries have the "
        "same default registry.");
  }
  std::shared_ptr<FunctionDefAndOpRegistration> function_def;
  {
    tf_shared_lock l(other.mu_);
    function_def = other.FindHelper(func);
  }
  if (!function_def) {
    return errors::InvalidArgument(
        "Cannot copy function '", func,
        "' because no function with that name exists in the other library.");
  }
  {
    mutex_lock l(mu_);
    std::shared_ptr<FunctionDefAndOpRegistration>& entry = function_defs_[func];
    if (entry) {
      if (!FunctionDefsEqual(entry->fdef, function_def->fdef)) {
        return errors::InvalidArgument(
            "Cannot copy function '", func,
            "' because a different function with the same name already "
            "exists.");
      }
    } else {
      entry = std::move(function_def);
    }
  }
  return Status::OK();
}