main_format_rate (xoff_t bytes, long millis, shortbuf *buf)
{
  xoff_t r = (xoff_t)(1.0 * bytes / (1.0 * millis / 1000.0));
  static shortbuf lbuf;

  main_format_bcnt (r, &lbuf);
  short_sprintf (*buf, "%s/s", lbuf.buf);
  return buf->buf;
}