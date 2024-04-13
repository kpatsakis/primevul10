TypedValue HHVM_FUNCTION(str_split, const String& str, int64_t split_length) {
  return tvReturn(StringUtil::Split(str, split_length));
}