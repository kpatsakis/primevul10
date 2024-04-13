inline int php_iptc_put1(req::ptr<File> /*file*/, int spool, unsigned char c,
                         unsigned char** spoolbuf) {
  if (spool > 0) {
    g_context->write((const char *)&c, 1);
  }

  if (spoolbuf) *(*spoolbuf)++ = c;

  return c;
}