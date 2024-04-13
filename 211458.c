int php_iptc_skip_variable(const req::ptr<File>& file,
                           int spool,
                           unsigned char **spoolbuf) {
  unsigned int length;
  int c1, c2;

  if ((c1 = php_iptc_get1(file, spool, spoolbuf)) == EOF) return M_EOI;

  if ((c2 = php_iptc_get1(file, spool, spoolbuf)) == EOF) return M_EOI;

  length = (((unsigned char) c1) << 8) + ((unsigned char) c2);

  length -= 2;

  while (length--) {
    if (php_iptc_get1(file, spool, spoolbuf) == EOF) return M_EOI;
  }
  return 0;
}