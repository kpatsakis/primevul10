Variant HHVM_FUNCTION(strtr,
                      const String& str,
                      const Variant& from,
                      const Variant& to /* = uninit_variant */) {
  if (str.empty()) {
    return str;
  }

  if (!to.isNull()) {
    return StringUtil::Translate(str, from.toString(), to.toString());
  }

  if (!from.isArray()) {
    raise_invalid_argument_warning("2nd argument: (not array)");
    return false;
  }

  int maxlen = 0;
  int minlen = -1;
  Array arr = from.toArray();

  if (arr.empty()) {
    // Nothing to translate
    return str;
  }

  for (ArrayIter iter(arr); iter; ++iter) {
    auto const search = iter.first().toString();
    auto const len = search.size();
    if (len < 1) return false;
    if (maxlen < len) maxlen = len;
    if (minlen == -1 || minlen > len) minlen = len;
  }

  if (arr.size() <= kBitsPerQword && maxlen <= 16) {
    return strtr_fast(str, arr, minlen, maxlen);
  }

  if (arr.size() < 1000) {
    const WuManberReplacement replacer(arr, minlen);
    return replacer.translate(str);
  }

  // wu manber cost is mostly in preprocessing the patterns into
  // tables.  this hash is much faster than initializing and most
  // codebases likely only have a few constant sets with more than
  // a thousand patterns.
  int64_t hash = 0;
  for (ArrayIter iter(arr); iter; ++iter) {
    String pattern = iter.first().toString();
    String replacement = iter.second().toString();
    hash = murmur_hash_64A(pattern.data(), pattern.size(), hash);
    hash = murmur_hash_64A(replacement.data(), replacement.size(), hash);
  }
  WuManberCache::ConstAccessor got;
  WuManberPtr replacer;
  if (wuManberCache.find(got, hash)) {
    replacer = *got;
  } else {
    replacer.reset(new WuManberReplacement(arr, minlen));
    wuManberCache.insert(hash, replacer);
  }
  return replacer->translate(str);
}