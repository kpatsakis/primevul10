Variant str_replace(const Variant& search, const Variant& replace,
                    const Variant& subject, int64_t& count, bool caseSensitive) {
  count = 0;
  if (subject.isArray()) {
    Array arr = subject.toArray();
    Array ret = Array::CreateDArray();
    int64_t c;
    for (ArrayIter iter(arr); iter; ++iter) {
      if (iter.second().isArray() || iter.second().is(KindOfObject)) {
        ret.set(iter.first(), iter.second());
        continue;
      }

      auto const replaced = str_replace(
        search, replace, iter.second().toString(), c, caseSensitive
      ).toString();
      ret.set(iter.first(), replaced);
      count += c;
    }
    return ret;
  }
  return str_replace(search, replace, subject.toString(), count,
                     caseSensitive);
}