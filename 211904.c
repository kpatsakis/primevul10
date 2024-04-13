int64_t HHVM_FUNCTION(strlen,
                      StringArg str) {
  return str.get()->size();
}