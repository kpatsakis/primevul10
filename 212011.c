int php_iptc_next_marker(const req::ptr<File>& file,
                         int spool,
                         unsigned char **spoolbuf) {
  int c;

  /* skip unimportant stuff */

  c = php_iptc_get1(file, spool, spoolbuf);

  if (c == EOF) return M_EOI;

  while (c != 0xff) {
    if ((c = php_iptc_get1(file, spool, spoolbuf)) == EOF) {
      return M_EOI; /* we hit EOF */
    }
  }

  /* get marker byte, swallowing possible padding */
  do {
    c = php_iptc_get1(file, 0, 0);
    if (c == EOF)
      return M_EOI;       /* we hit EOF */
    else if (c == 0xff)
      php_iptc_put1(file, spool, (unsigned char)c, spoolbuf);
  } while (c == 0xff);

  return (unsigned int) c;
}