int change_config_line(LOGBOOK *lbs, char *option, char *old_value, char *new_value) {
   int fh, i, j, n, length, bufsize;
   char str[NAME_LENGTH], *buf, *buf2, *p1, *p2, *p3;
   char list[MAX_N_LIST][NAME_LENGTH], line[NAME_LENGTH];

   fh = open(config_file, O_RDWR | O_BINARY, 644);
   if (fh < 0) {
      sprintf(str, loc("Cannot open file <b>%s</b>"), config_file);
      strcat(str, ": ");
      strcat(str, strerror(errno));
      show_error(str);
      return 0;
   }

   /* read previous contents */
   length = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   bufsize = 2 * (length + strlen(new_value) + 10);
   buf = xmalloc(bufsize);
   read(fh, buf, length);
   buf[length] = 0;

   /* find location of option */
   p1 = (char *) find_param(buf, lbs->name, option);
   if (p1 == NULL)
      return 0;

   p2 = strchr(p1, '=');
   if (p2 == 0)
      return 0;

   p2++;
   while (*p2 == ' ' || *p2 == '\t')
      p2++;

   strlcpy(line, p2, sizeof(line));
   if (strchr(line, '\r'))
      *strchr(line, '\r') = 0;
   if (strchr(line, '\n'))
      *strchr(line, '\n') = 0;
   n = strbreak(line, list, MAX_N_LIST, ",", FALSE);

   /* save tail */
   p3 = strchr(p2, '\n');
   if (p3 && *(p3 - 1) == '\r')
      p3--;

   buf2 = NULL;
   if (p3)
      buf2 = xstrdup(p3);

   if (old_value[0]) {
      for (i = 0; i < n; i++) {
         if (strieq(old_value, list[i])) {
            if (new_value[0]) {
               /* rename value */
               strcpy(list[i], new_value);
            } else {
               /* delete value */
               for (j = i; j < n - 1; j++)
                  strcpy(list[j], list[j + 1]);
               n--;
            }
            break;
         }
      }
   } else {
      if (n < MAX_N_LIST)
         strcpy(list[n++], new_value);
   }

   /* write new option list */
   for (i = 0; i < n; i++) {
      strcpy(p2, list[i]);
      if (i < n - 1)
         strcat(p2, ", ");
      p2 += strlen(p2);
   }

   /* append tail */
   if (buf2) {
      strlcat(p2, buf2, length + strlen(new_value) + 10);
      xfree(buf2);
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

   return 1;
}