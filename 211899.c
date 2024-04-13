TypedValue HHVM_FUNCTION(strtok, const String& str, const Variant& token) {
  return tvReturn(strtok(str, token));
}