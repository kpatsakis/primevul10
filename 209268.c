void CharCodeToUnicodeCache::add(CharCodeToUnicode *ctu) {
  int i;

  if (cache[size - 1]) {
    cache[size - 1]->decRefCnt();
  }
  for (i = size - 1; i >= 1; --i) {
    cache[i] = cache[i - 1];
  }
  cache[0] = ctu;
  ctu->incRefCnt();
}