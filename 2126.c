Status FunctionLibraryDefinition::AddLibrary(
    const FunctionDefLibrary& lib_def) {
  // Remember the funcs and grads that we added successfully so that
  // we can roll them back on error.
  mutex_lock l(mu_);
  std::vector<string> funcs;
  std::vector<string> funcs_with_grads;
  Status s;
  bool added;
  for (const FunctionDef& fdef : lib_def.function()) {
    s = AddFunctionDefHelper(fdef, /*stack_traces=*/{}, &added);
    if (!s.ok()) {
      Status remove_status = Remove(funcs, funcs_with_grads);
      if (!remove_status.ok()) {
        return remove_status;
      }
      return s;
    }
    if (added) {
      funcs.push_back(fdef.signature().name());
    }
  }
  for (const GradientDef& grad : lib_def.gradient()) {
    s = AddGradientDefHelper(grad, &added);
    if (!s.ok()) {
      Status remove_status = Remove(funcs, funcs_with_grads);
      if (!remove_status.ok()) {
        return remove_status;
      }
      return s;
    }
    if (added) {
      funcs_with_grads.push_back(grad.function_name());
    }
  }
  return Status::OK();
}