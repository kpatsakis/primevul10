int rename_logbook(LOGBOOK *lbs, char *new_name) {
   int fh, i, length, bufsize;
   char *buf, *buf2, *p1, *p2;
   char str[256], lb_dir[256], old_dir[256], new_dir[256];

   fh = open(config_file, O_RDWR | O_BINARY, 644);
   if (fh < 0) {
      sprintf(str, loc("Cannot open file <b>%s</b>"), config_file);
      strcat(str, ": ");
      strcat(str, strerror(errno));
      show_error(str);
      return 0;
   }

   /* rename logbook file */
   if (!getcfg(lbs->name, "Subdir", str, sizeof(str))) {
      strlcpy(lb_dir, logbook_dir, sizeof(lb_dir));
      if (lb_dir[strlen(lb_dir) - 1] != DIR_SEPARATOR)
         strlcat(lb_dir, DIR_SEPARATOR_STR, sizeof(lb_dir));

      sprintf(old_dir, "%s%s", lb_dir, lbs->name);
      sprintf(new_dir, "%s%s", lb_dir, new_name);
      rename(old_dir, new_dir);
   }

   /* change logbook name in groups */
   change_logbook_in_group(lbs, new_name);

   /* read previous contents */
   length = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   bufsize = 2 * (length + strlen(new_name) + 10);
   buf = xmalloc(bufsize);
   read(fh, buf, length);
   buf[length] = 0;

   /* find logbook config */
   p1 = (char *) find_section(buf, lbs->name);
   p2 = strchr(p1, ']');
   if (p2 == NULL) {
      close(fh);
      xfree(buf);
      show_error(loc("Syntax error in config file"));
      return 0;
   }
   p2++;

   /* save tail */
   buf2 = xstrdup(p2);

   /* replace logbook name */
   sprintf(p1, "[%s]", new_name);

   strlcat(p1, buf2, length + strlen(new_name) + 1);
   xfree(buf2);

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