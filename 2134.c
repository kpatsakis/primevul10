Status FunctionLibraryDefinition::ReplaceGradient(const GradientDef& grad) {
  mutex_lock l(mu_);
  bool added;
  TF_RETURN_IF_ERROR(RemoveGradient(grad.function_name()));
  TF_RETURN_IF_ERROR(AddGradientDefHelper(grad, &added));
  return Status::OK();
}