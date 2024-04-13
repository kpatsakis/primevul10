ALWAYS_INLINE String& asStrRef(tv_lval tv) {
  assertx(tvIsPlausible(*tv));
  assertx(isStringType(type(tv)));
  type(tv) = KindOfString;
  return reinterpret_cast<String&>(val(tv).pstr);
}