TypedValue HHVM_FUNCTION(strrpos,
                         const String& haystack,
                         const Variant& needle,
                         int offset /* = 0 */) {
  if (!is_valid_strrpos_args(haystack, needle, offset)) {
    return make_tv<KindOfBoolean>(false);
  }
  int pos;
  if (needle.isString()) {
    pos = haystack.rfind(needle.toString(), offset);
  } else {
    pos = haystack.rfind(needle.toByte(), offset);
  }
  if (pos >= 0) return make_tv<KindOfInt64>(pos);
  return make_tv<KindOfBoolean>(false);
}