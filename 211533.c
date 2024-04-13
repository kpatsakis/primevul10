ALWAYS_INLINE const String& asCStrRef(tv_rval tv) {
  assertx(tvIsPlausible(*tv));
  assertx(isStringType(type(tv)));
  return reinterpret_cast<const String&>(val(tv).pstr);
}