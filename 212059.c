TypedValue HHVM_FUNCTION(stristr,
                         const String& haystack,
                         const Variant& needle,
                         bool before_needle /* = false */) {
  auto const tv = HHVM_FN(stripos)(haystack, needle);
  auto const& ret = tvAsCVarRef(&tv);
  assertx(!isRefcountedType(tv.m_type));

  if (same(ret, false)) {
    return make_tv<KindOfBoolean>(false);
  }
  if (before_needle) {
    return tvReturn(haystack.substr(0, ret.toInt32()));
  }
  return tvReturn(haystack.substr(ret.toInt32()));
}