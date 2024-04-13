int vspprintf(char** pbuf, size_t /*max_len*/, const char* format, ...) {
  int len;
  va_list ap;
  va_start(ap, format);
  len = xbuf_format_converter(pbuf, format, ap);
  va_end(ap);
  return len;
}