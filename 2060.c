Status FunctionLibraryDefinition::AddFunctionDef(
    const FunctionDef& fdef, const StackTracesMap& stack_traces) {
  mutex_lock l(mu_);
  bool added;
  return AddFunctionDefHelper(fdef, stack_traces, &added);
}