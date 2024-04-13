static char *php_strndup_impl(const char* s, uint32_t length
#ifdef IM_MEMORY_CHECK
, int ln
#endif
  ) {
  char *p;

#ifdef IM_MEMORY_CHECK
  p = (char *)s_ima->imMalloc((length+1), ln);
#else
  p = (char *)s_ima->imMalloc((length+1));
#endif
  CHECK_ALLOC_R(p, length+1, nullptr);
  memcpy(p, s, length);
  p[length] = 0;
  return p;
}