Status FunctionLibraryDefinition::AddGradientDef(const GradientDef& grad) {
  mutex_lock l(mu_);
  bool added;
  return AddGradientDefHelper(grad, &added);
}