int is_ascii(char *file_name) {
   int i, fh, length;
   unsigned char *buf;

   fh = open(file_name, O_RDONLY | O_BINARY);
   if (fh < 0)
      return FALSE;
   lseek(fh, 0, SEEK_END);
   length = TELL(fh);
   lseek(fh, 0, SEEK_SET);
   if (length > 1000)
      length = 1000;
   buf = xmalloc(length);
   read(fh, buf, length);
   close(fh);

   for (i = 0; i < length; i++) {
      if (buf[i] < 32 && buf[i] != '\r' && buf[i] != '\n' && buf[i] != '\t') {
         xfree(buf);
         return FALSE;
      }
   }

   xfree(buf);
   return TRUE;
}