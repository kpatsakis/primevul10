inline int php_iptc_read_remaining(const req::ptr<File>& file,
                                   int spool,
                                   unsigned char **spoolbuf) {
  while (php_iptc_get1(file, spool, spoolbuf) != EOF) continue;

  return M_EOI;
}