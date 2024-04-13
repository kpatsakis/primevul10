static unsigned short php_read2(const req::ptr<File>& stream) {
  unsigned char *a;
  String str = stream->read(2);
  /* just return 0 if we hit the end-of-file */
  if (str.length() != 2) return 0;
  a = (unsigned char *)str.c_str();
  return (((unsigned short)a[0]) << 8) + ((unsigned short)a[1]);
}