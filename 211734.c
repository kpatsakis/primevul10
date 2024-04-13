int preg_replace_callback(Variant& result,
                          const Variant& pattern,
                          const Variant& callback,
                          const Variant& subject,
                          int limit /* = -1 */) {
  int64_t count;
  result = preg_replace_impl(pattern, callback, subject,
                             limit, &count, true, false);
  return count;
}