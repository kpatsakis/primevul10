String stringForEachFast(const String& str, Op action) {
  if (str.empty()) {
    return str;
  }

  return stringForEach<false>(str.size(), str, action);
}