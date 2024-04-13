int save_admin_config(char *section, char *buffer, char *error) {
   int fh, i, length;
   char *buf, *buf2, *p1, *p2;

   error[0] = 0;

   fh = open(config_file, O_RDWR | O_BINARY, 644);
   if (fh < 0) {
      sprintf(error, loc("Cannot open file <b>%s</b>"), config_file);
      strcat(error, ": ");
      strcat(error, strerror(errno));
      return 0;
   }

   /* read previous contents */
   length = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   buf = xmalloc(length + strlen(buffer) + 10);
   read(fh, buf, length);
   buf[length] = 0;

   /* find previous logbook config */
   p1 = (char *) find_section(buf, section);
   p2 = (char *) find_next_section(p1 + 1);

   /* save tail */
   buf2 = NULL;

   if (p2)
      buf2 = xstrdup(p2);

   /* combine old and new config */
   sprintf(p1, "[%s]\r\n", section);
   strcat(p1, buffer);
   strcat(p1, "\r\n\r\n");

   if (p2) {
      strlcat(p1, buf2, length + strlen(buffer) + 1);
      xfree(buf2);
   }

   adjust_crlf(buf, length + strlen(buffer) + 10);

   lseek(fh, 0, SEEK_SET);
   i = write(fh, buf, strlen(buf));
   if (i < (int) strlen(buf)) {
      sprintf(error, loc("Cannot write to <b>%s</b>"), config_file);
      strcat(error, ": ");
      strcat(error, strerror(errno));
      close(fh);
      xfree(buf);
      return 0;
   }

   TRUNCATE(fh);

   close(fh);
   xfree(buf);

   /* force re-read of config file */
   check_config_file(TRUE);

   return 1;
}