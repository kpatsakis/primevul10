int create_logbook(LOGBOOK *oldlbs, char *logbook, char *templ) {
   int fh, i, length, bufsize, templ_length;
   char *buf, *p1, *p2, str[256];

   fh = open(config_file, O_RDWR | O_BINARY, 644);
   if (fh < 0) {
      sprintf(str, loc("Cannot open file <b>%s</b>"), config_file);
      strcat(str, ": ");
      strcat(str, strerror(errno));
      show_error(str);
      return 0;
   }

   /* add logbook to current group */
   add_logbook_to_group(oldlbs, logbook);

   /* read previous contents */
   length = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   bufsize = 2 * (2 * length + 1);
   buf = (char *) xmalloc(bufsize);
   read(fh, buf, length);
   buf[length] = 0;
   templ_length = 0;
   p2 = NULL;

   /* find template logbook */
   if (templ[0]) {
      p1 = (char *) find_section(buf, templ);
      p2 = (char *) find_next_section(p1 + 1);
   } else
      p1 = NULL;

   if (p1) {
      p1 = strchr(p1, ']');
      if (p1)
         while (*p1 == ']' || *p1 == '\r' || *p1 == '\n')
            p1++;

      if (p2)
         templ_length = p2 - p1;
      else
         templ_length = strlen(p1);
   }

   /* insert single blank line after last logbook */
   p2 = buf + strlen(buf) - 1;

   while (p2 > buf && (*p2 == '\r' || *p2 == '\n' || *p2 == ' ' || *p2 == '\t')) {
      *p2 = 0;
      p2--;
   }
   if (p2 > buf)
      p2++;

   strcat(p2, "\r\n\r\n[");
   strcat(p2, logbook);
   strcat(p2, "]\r\n");
   if (p1) {
      p2 = buf + strlen(buf);
      strncpy(p2, p1, templ_length);
      p2[templ_length] = 0;
   }

   adjust_crlf(buf, bufsize);

   lseek(fh, 0, SEEK_SET);
   i = write(fh, buf, strlen(buf));
   if (i < (int) strlen(buf)) {
      sprintf(str, loc("Cannot write to <b>%s</b>"), config_file);
      strcat(str, ": ");
      strcat(str, strerror(errno));
      show_error(str);
      close(fh);
      xfree(buf);
      return 0;
   }

   TRUNCATE(fh);

   close(fh);
   xfree(buf);

   /* force re-read of config file */
   check_config_file(TRUE);
   el_index_logbooks();

   return 1;
}