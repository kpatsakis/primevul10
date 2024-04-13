Variant preg_match_all(const String& pattern, const String& subject,
                       Variant* matches /* = nullptr */,
                       int flags /* = 0 */, int offset /* = 0 */) {
  return preg_match_all(pattern.get(), subject.get(), matches, flags, offset);
}