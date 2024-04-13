inline int php_iptc_get1(const req::ptr<File>& file,
                         int spool,
                         unsigned char **spoolbuf) {
  int c;
  char cc;

  c = file->getc();

  if (c == EOF) return EOF;

  if (spool > 0) {
    cc = c;
    g_context->write((const char *)&cc, 1);
  }

  if (spoolbuf) *(*spoolbuf)++ = c;

  return c;
}