int preg_replace(Variant& result,
                 const Variant& pattern,
                 const Variant& replacement,
                 const Variant& subject,
                 int limit /* = -1 */) {
  int64_t count;
  result = preg_replace_impl(pattern, replacement, subject,
                             limit, &count, false, false);
  return count;
}