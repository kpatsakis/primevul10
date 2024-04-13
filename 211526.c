static int php_vspprintf_ap(char **pbuf, size_t max_len,
                            const char *fmt, va_list ap) {
  char *buf;
  int len = vspprintf_ap(&buf, max_len, fmt, ap);
  if (buf) {
#ifdef IM_MEMORY_CHECK
    *pbuf = php_strndup_impl(buf, len, __LINE__);
#else
    *pbuf = php_strndup_impl(buf, len);
#endif
    free(buf);
  }
  return len;
}