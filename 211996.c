TypedValue HHVM_FUNCTION(strrchr,
                         const String& haystack,
                         const Variant& needle) {
  if (haystack.size() == 0) {
    return make_tv<KindOfBoolean>(false);
  }

  int pos;
  if (needle.isString() && needle.toString().size() > 0) {
    pos = haystack.rfind(needle.toString().data()[0], false);
  } else {
    pos = haystack.rfind(needle.toByte(), false);
  }
  if (pos < 0) return make_tv<KindOfBoolean>(false);
  return tvReturn(haystack.substr(pos));
}