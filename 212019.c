static unsigned short php_read4(const req::ptr<File>& stream) {
  unsigned char *a;
  String str = stream->read(4);
  /* just return 0 if we hit the end-of-file */
  if (str.length() != 4) return 0;
  a = (unsigned char *)str.c_str();
  return (((unsigned int)a[0]) << 24)
       + (((unsigned int)a[1]) << 16)
       + (((unsigned int)a[2]) <<  8)
       + (((unsigned int)a[3]));
}