Variant str_ireplace(const Variant& search, const Variant& replace,
                     const Variant& subject, int64_t& count) {
  Variant ret = str_replace(search, replace, subject, count, false);
  return ret;
}