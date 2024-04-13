void adjust_crlf(char *buffer, int bufsize) {
   char *p;
   UNUSED(bufsize);

#ifdef OS_UNIX

   /* convert \r\n -> \n */
   p = buffer;
   while ((p = strstr(p, "\r\n")) != NULL) {
      memmove(p, p + 1, strlen(p + 1) + 1);     // strcpy() gives error under Ubuntu
   }
#else

                                                                                                                           char *tmpbuf;

   assert(bufsize);
   tmpbuf = xmalloc(bufsize);

   /* convert \n -> \r\n */
   p = buffer;
   while ((p = strstr(p, "\n")) != NULL) {

      if (p > buffer && *(p - 1) == '\r') {
         p++;
         continue;
      }

      if ((int) strlen(buffer) + 2 >= bufsize) {
         xfree(tmpbuf);
         return;
      }

      strlcpy(tmpbuf, p, bufsize);
      *(p++) = '\r';
      strlcpy(p, tmpbuf, bufsize - (p - buffer));
      p++;
   }

   xfree(tmpbuf);
#endif
}