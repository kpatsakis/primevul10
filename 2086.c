Status FunctionLibraryDefinition::AddGradientDefHelper(const GradientDef& grad,
                                                       bool* added) {
  *added = false;
  string* entry = &func_grad_[grad.function_name()];
  if (!entry->empty()) {
    if (*entry != grad.gradient_func()) {
      return errors::InvalidArgument(
          "Cannot assign gradient function '", grad.gradient_func(), "' to '",
          grad.function_name(), "' because it already has gradient function ",
          "'", *entry, "'");
    }
    // Ignore duplicate GradientDefs
    return Status::OK();
  }
  *entry = grad.gradient_func();
  *added = true;
  return Status::OK();
}