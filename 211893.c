String HHVM_FUNCTION(gettype, const Variant& v) {
  if (v.getType() == KindOfResource && v.toCResRef().isInvalid()) {
    return s_unknown_type;
  }
  /* Although getDataTypeString also handles the null type, it returns "null"
   * (lower case). Unfortunately, PHP returns "NULL" (upper case) for
   * gettype(). So we make an exception here. */
  if (v.isNull()) {
    return s_NULL;
  }

  // OH NO. This string could be used by logic in Hack, so we can't do the
  // sensible thing here and return "varray" or "darray" for dvarrays.
  if (isArrayType(v.getType())) {
    maybe_raise_array_serialization_notice(SerializationSite::Gettype,
                                           v.getArrayData());
    return s_array;
  }
  if (isArrayLikeType(v.getType()) && v.getArrayData()->isLegacyArray()) {
    return s_array;
  }
  if (RuntimeOption::EvalClassAsStringGetType &&
      (v.isLazyClass() || v.isClass())) {
    return s_string;
  }
  return getDataTypeString(v.getType());
}