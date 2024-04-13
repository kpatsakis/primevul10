pcre_cache_entry::~pcre_cache_entry() {
  if (extra) {
#if PCRE_MAJOR < 8 || (PCRE_MAJOR == 8 && PCRE_MINOR < 20)
    free(extra);
#else
    pcre_free_study(extra);
#endif
  }
  free(subpat_names);
  pcre_free(re);
}