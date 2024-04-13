bool HHVM_FUNCTION(is_array, const Variant& v) {
  if (RuntimeOption::EvalLogOnIsArrayFunction) {
    raise_notice("call to deprecated builtin is_array()");
  }
  return is_any_array(v.asTypedValue());
}