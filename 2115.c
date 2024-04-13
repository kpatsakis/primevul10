Status FunctionLibraryDefinition::ReplaceFunction(
    const string& func, const FunctionDef& fdef,
    const StackTracesMap& stack_traces) {
  mutex_lock l(mu_);
  bool added;
  TF_RETURN_IF_ERROR(RemoveFunctionHelper(func));
  TF_RETURN_IF_ERROR(AddFunctionDefHelper(fdef, stack_traces, &added));
  return Status::OK();
}