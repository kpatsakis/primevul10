static char *php_strdup_impl(const char* s
#ifdef IM_MEMORY_CHECK
, int ln
#endif
  ) {
#ifdef IM_MEMORY_CHECK
  return php_strndup_impl(s, strlen(s), ln);
#else
  return php_strndup_impl(s, strlen(s));
#endif
}