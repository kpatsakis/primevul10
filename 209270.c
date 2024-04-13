CharCodeToUnicodeCache::~CharCodeToUnicodeCache() {
  int i;

  for (i = 0; i < size; ++i) {
    if (cache[i]) {
      cache[i]->decRefCnt();
    }
  }
  gfree(cache);
}