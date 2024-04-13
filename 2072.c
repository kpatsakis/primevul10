Status FunctionLibraryDefinition::AddFunctionDefHelper(
    const FunctionDef& fdef, const StackTracesMap& stack_traces, bool* added) {
  *added = false;
  std::shared_ptr<FunctionDefAndOpRegistration>& entry =
      function_defs_[fdef.signature().name()];
  if (entry) {
    if (!FunctionDefsEqual(entry->fdef, fdef)) {
      return errors::InvalidArgument(
          "Cannot add function '", fdef.signature().name(),
          "' because a different function with the same name already "
          "exists.");
    }
    // Ignore duplicate FunctionDefs.
    return Status::OK();
  }
  const OpDef* op_def;
  if (default_registry_->LookUpOpDef(fdef.signature().name(), &op_def).ok()) {
    return errors::InvalidArgument(
        "Cannot add function '", fdef.signature().name(),
        "' because an op with the same name already exists.");
  }
  entry = std::make_shared<FunctionDefAndOpRegistration>(fdef, stack_traces);
  *added = true;
  return Status::OK();
}