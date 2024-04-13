TypedValue HHVM_FUNCTION(substr_count,
                         const String& haystack,
                         const String& needle,
                         int offset /* = 0 */,
                         int length /* = 0x7FFFFFFF */) {
  int lenNeedle = needle.size();
  if (lenNeedle == 0) {
    raise_invalid_argument_warning("needle: (empty)");
    return make_tv<KindOfBoolean>(false);
  }

  if (offset < 0 || offset > haystack.size()) {
    raise_invalid_argument_warning("offset: (out of range)");
    return make_tv<KindOfBoolean>(false);
  }
  if (length == 0x7FFFFFFF) {
    length = haystack.size() - offset;
  } else if (length <= 0 || length > haystack.size() - offset) {
    raise_invalid_argument_warning("length: (out of range)");
    return make_tv<KindOfBoolean>(false);
  }

  int count = 0;
  int posMax = offset + length - lenNeedle;
  for (int pos = haystack.find(needle, offset);
       pos != -1 && pos <= posMax;
       pos = haystack.find(needle, pos + lenNeedle)) {
    ++count;
  }
  return make_tv<KindOfInt64>(count);
}