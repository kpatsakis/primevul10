void pcre_dump_cache(folly::File& file) {
  s_pcreCache.dump(file);
}