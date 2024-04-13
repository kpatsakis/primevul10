bool HHVM_FUNCTION(HH_is_any_array, const Variant& val) {
  if (tvIsClsMeth(val.asTypedValue())) {
    if (RuntimeOption::EvalIsCompatibleClsMethType) {
      if (RuntimeOption::EvalIsVecNotices) {
        raise_notice(Strings::CLSMETH_COMPAT_IS_ANY_ARR);
      }
      return true;
    }
    return false;
  }

  return tvIsArrayLike(val.asTypedValue()) || tvIsClsMeth(val.asTypedValue());
}