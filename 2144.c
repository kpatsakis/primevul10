Status FunctionLibraryDefinition::RemoveFunction(const string& func) {
  mutex_lock l(mu_);
  TF_RETURN_IF_ERROR(RemoveFunctionHelper(func));
  return Status::OK();
}