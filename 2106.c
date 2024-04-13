Status FunctionLibraryDefinition::AddLibrary(
    const FunctionLibraryDefinition& other) {
  // Clone `other` to ensure thread-safety (grabbing `other`'s lock for
  // the duration of the function could lead to deadlock).
  FunctionLibraryDefinition clone(other);
  mutex_lock l(mu_);
  mutex_lock l2(clone.mu_);
  // Remember the funcs and grads that we added successfully so that
  // we can roll them back on error.
  std::vector<string> funcs;
  std::vector<string> funcs_with_grads;
  Status s;
  bool added;
  for (auto iter : clone.function_defs_) {
    s = AddHelper(iter.second, &added);
    if (!s.ok()) {
      Status remove_status = Remove(funcs, funcs_with_grads);
      if (!remove_status.ok()) {
        return remove_status;
      }
      return s;
    }
    if (added) {
      funcs.push_back(iter.second->fdef.signature().name());
    }
  }
  for (auto iter : clone.func_grad_) {
    GradientDef grad;
    grad.set_function_name(iter.first);
    grad.set_gradient_func(iter.second);
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