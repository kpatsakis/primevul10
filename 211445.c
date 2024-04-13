ALWAYS_INLINE TypedValue empty_string_tv() {
  return make_tv<KindOfPersistentString>(staticEmptyString());
}