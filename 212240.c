int vspprintf_ap(char** pbuf, size_t /*max_len*/, const char* format,
                 va_list ap) {
  int len;
  len = xbuf_format_converter(pbuf, format, ap);
  return len;
}