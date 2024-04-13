Variant preg_match(const String& pattern, const String& subject,
                   Variant* matches /* = nullptr */, int flags /* = 0 */,
                   int offset /* = 0 */) {
  return preg_match(pattern.get(), subject.get(), matches, flags, offset);
}