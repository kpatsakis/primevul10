std::vector<string> FunctionLibraryDefinition::ListFunctionNames() const {
  std::vector<string> function_names;
  tf_shared_lock l(mu_);
  function_names.reserve(function_defs_.size());
  for (const auto& it : function_defs_) {
    function_names.emplace_back(it.first);
  }
  return function_names;
}