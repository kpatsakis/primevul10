int delete_logbook(LOGBOOK *lbs, char *error) {
   int fh, i, length;
   char *buf, *p1, *p2;

   error[0] = 0;

   fh = open(config_file, O_RDWR | O_BINARY, 644);
   if (fh < 0) {
      sprintf(error, loc("Cannot open file <b>%s</b>"), config_file);
      strcat(error, ": ");
      strcat(error, strerror(errno));
      return 0;
   }

   /* remove logbook name in groups */
   change_logbook_in_group(lbs, "");

   /* read previous contents */
   length = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   buf = xmalloc(length + 1);
   read(fh, buf, length);
   buf[length] = 0;

   /* find logbook config */
   p1 = (char *) find_section(buf, lbs->name);
   p2 = (char *) find_next_section(p1 + 1);

   if (p2) {
      i = strlen(p2) + 1;
      strlcpy(p1, p2, i);
   } else
      *p1 = 0;

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
   el_index_logbooks();

   return 1;
}