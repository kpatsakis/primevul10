Status FunctionLibraryDefinition::AddHelper(
    std::shared_ptr<FunctionDefAndOpRegistration> registration, bool* added) {
  *added = false;
  std::shared_ptr<FunctionDefAndOpRegistration>& entry =
      function_defs_[registration->fdef.signature().name()];
  if (entry) {
    if (!FunctionDefsEqual(entry->fdef, registration->fdef)) {
      return errors::InvalidArgument(
          "Cannot add function '", registration->fdef.signature().name(),
          "' because a different function with the same name already "
          "exists.");
    }
    // Ignore duplicate FunctionDefs.
    return Status::OK();
  }
  const OpDef* op_def;
  if (default_registry_
          ->LookUpOpDef(registration->fdef.signature().name(), &op_def)
          .ok()) {
    return errors::InvalidArgument(
        "Cannot add function '", registration->fdef.signature().name(),
        "' because an op with the same name already exists.");
  }
  entry = std::move(registration);
  *added = true;
  return Status::OK();
}