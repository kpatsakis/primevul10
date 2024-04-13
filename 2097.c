string FunctionLibraryDefinition::UniqueFunctionName(StringPiece prefix) const {
  tf_shared_lock l(mu_);
  int index = 0;
  string name = strings::StrCat(prefix, index);
  while (function_defs_.find(name) != function_defs_.end()) {
    ++index;
    name = strings::StrCat(prefix, index);
  }
  return name;
}