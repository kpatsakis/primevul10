int adjust_config(char *url) {
   int fh, i, length;
   char *buf, *buf2, *p1, *p2;
   char str[256];

   fh = open(config_file, O_RDWR | O_BINARY, 0644);
   if (fh < 0) {
      sprintf(str, loc("Cannot open file <b>%s</b>"), config_file);
      strcat(str, ": ");
      strcat(str, strerror(errno));
      eputs(str);
      return 0;
   }

   /* read previous contents */
   length = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   buf = (char *) xmalloc(2 * length + 1000);
   read(fh, buf, length);
   buf[length] = 0;

   /* add mirror server */
   p1 = stristr(buf, "Mirror server =");
   if (p1 != NULL) {
      p2 = strchr(p1, '\n');
      if (p2 && *(p2 - 1) == '\r')
         p2--;
   } else {
      p1 = strstr(buf, "[global]");
      if (p1 == NULL) {
         eputs("Cannot find [global] section in config file.");
         return 0;
      }
      p1 = strchr(p1, '\n');
      while (*p1 == '\n' || *p1 == '\r')
         p1++;

      p2 = p1;
   }

   /* save tail */
   buf2 = NULL;
   if (p2)
      buf2 = xstrdup(p2);

   sprintf(p1, "Mirror server = %s\r\n", url);
   strlcat(p1, buf2, length + 1000);
   xfree(buf2);
   eprintf("Option \"Mirror server = %s\" added to config file.\n", url);

   /* outcomment "URL = xxx" */
   p1 = strstr(buf, "URL =");
   if (p1 != NULL) {

      /* save tail */
      buf2 = xstrdup(p1);

      /* add comment */
      sprintf(p1, "; Following line has been outcommented after cloning\r\n");
      strlcat(p1, "; ", length + 1000);
      strlcat(p1, buf2, length + 1000);
      xfree(buf2);

      eputs("Option \"URL = xxx\" has been outcommented from config file.");
   }

   adjust_crlf(buf, 2 * length + 1000);

   lseek(fh, 0, SEEK_SET);
   i = write(fh, buf, strlen(buf));
   if (i < (int) strlen(buf)) {
      sprintf(str, loc("Cannot write to <b>%s</b>"), config_file);
      strcat(str, ": ");
      strcat(str, strerror(errno));
      eputs(str);
      close(fh);
      xfree(buf);
      return 0;
   }

   TRUNCATE(fh);

   close(fh);
   xfree(buf);

   return 1;
}