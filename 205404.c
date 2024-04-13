char *re_eprint(int err)
{
  static char epbuf[100];
  size_t len __attribute__((unused))=
          my_regerror(REG_ITOA|err, (my_regex_t *)NULL, epbuf, sizeof(epbuf));
  assert(len <= sizeof(epbuf));
  return(epbuf);
}