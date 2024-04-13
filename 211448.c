Variant str_replace(const Variant& search, const Variant& replace,
                    const String& subject, int64_t& count, bool caseSensitive) {
  count = 0;
  if (search.isArray()) {
    String ret = subject;
    int c = 0;

    Array searchArr = search.toArray();
    if (replace.isArray()) {
      Array replArr = replace.toArray();
      ArrayIter replIter(replArr);
      for (ArrayIter iter(searchArr); iter; ++iter) {
        if (replIter) {
          ret = string_replace(ret, iter.second().toString(),
                               replIter.second().toString(),
                               c, caseSensitive);
          ++replIter;
        } else {
          ret = string_replace(ret, iter.second().toString(),
                               "", c, caseSensitive);
        }
        count +=c;
      }
      return ret;
    }

    String repl = replace.toString();
    for (ArrayIter iter(searchArr); iter; ++iter) {
      ret = string_replace(ret, iter.second().toString(), repl, c,
                           caseSensitive);
      count += c;
    }
    return ret;
  }

  int icount;
  auto ret = string_replace(subject, search.toString(), replace.toString(),
                            icount, caseSensitive);
  count = icount;
  return ret;
}