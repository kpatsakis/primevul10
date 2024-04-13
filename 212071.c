Variant preg_match_all(const StringData* pattern, const StringData* subject,
                       Variant* matches /* = nullptr */,
                       int flags /* = 0 */, int offset /* = 0 */) {
  return preg_match_impl(pattern, subject, matches, flags, offset, true);
}