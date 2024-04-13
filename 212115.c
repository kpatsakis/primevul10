TypedValue HHVM_FUNCTION(strpos,
                         const String& haystack,
                         const Variant& needle,
                         int offset /* = 0 */) {
  if (offset < 0 || offset > haystack.size()) {
    raise_warning("Offset not contained in string");
    return make_tv<KindOfBoolean>(false);
  }
  int pos;
  if (needle.isString()) {
    String n(needle.toString());
    if (n.length() == 0) {
      raise_warning("Empty delimiter");
      return make_tv<KindOfBoolean>(false);
    }
    pos = haystack.find(n, offset);
  } else {
    pos = haystack.find(needle.toByte(), offset);
  }
  if (pos >= 0) return make_tv<KindOfInt64>(pos);
  return make_tv<KindOfBoolean>(false);
}