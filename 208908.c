int is_full_html(char *file_name) {
   char *str, *p;
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

   str = xstrdup((char *) buf);

   for (i = 0; i < (int) strlen((char *) buf); i++)
      str[i] = toupper(buf[i]);
   str[i] = 0;

   xfree(buf);

   for (i = 0; full_html_tags[i][0]; i++) {
      p = strstr(str, full_html_tags[i]);
      if (p && strchr(p, '>') && (p == str || (p > str && *(p - 1) != '\\'))) {
         xfree(str);
         return TRUE;
      }
   }

   xfree(str);
   return FALSE;
}