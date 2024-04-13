static int php_vspprintf(char **pbuf, size_t max_len,
                         const char *fmt, ...) {
  va_list arglist;
  char *buf;
  va_start(arglist, fmt);
  int len = vspprintf_ap(&buf, max_len, fmt, arglist);
  if (buf) {
#ifdef IM_MEMORY_CHECK
    *pbuf = php_strndup_impl(buf, len, __LINE__);
#else
    *pbuf = php_strndup_impl(buf, len);
#endif
    free(buf);
  }
  va_end(arglist);
  return len;
}