int set_attributes(LOGBOOK *lbs, char attributes[][NAME_LENGTH], int n) {
   int fh, i, length, size;
   char str[NAME_LENGTH], *buf, *buf2, *p1, *p2, *p3;

   fh = open(config_file, O_RDWR | O_BINARY, 0644);
   if (fh < 0) {
      sprintf(str, loc("Cannot open file <b>%s</b>"), config_file);
      strcat(str, ": ");
      strcat(str, strerror(errno));
      show_error(str);
      return 0;
   }

   /* determine length of attributes */
   for (i = size = 0; i < n; i++)
      size += strlen(attributes[i]) + 2;

   /* read previous contents */
   length = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   buf = xmalloc(length + size + 3);
   read(fh, buf, length);
   buf[length] = 0;

   /* find location of attributes */
   p1 = (char *) find_param(buf, lbs->name, "Attributes");
   if (p1 == NULL) {
      sprintf(str, loc("No 'Attributes' option present in %s"), config_file);
      show_error(str);
      return 0;
   }

   p2 = strchr(p1, '\n');
   if (p2 && *(p2 - 1) == '\r')
      p2--;

   /* save tail */
   buf2 = NULL;
   if (p2)
      buf2 = xstrdup(p2);

   /* add list */
   p3 = strchr(p1, '=');
   if (p3 == NULL)
      return 0;
   p3++;
   while (*p3 == ' ')
      p3++;

   for (i = 0; i < n - 1; i++) {
      sprintf(p3, "%s, ", attributes[i]);
      p3 += strlen(p3);
   }
   sprintf(p3, "%s", attributes[i]);

   if (p2) {
      strlcat(buf, buf2, length + size + 3);
      xfree(buf2);
   }

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

   return 1;
}