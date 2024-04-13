int add_attribute_option(LOGBOOK *lbs, char *attrname, char *attrvalue, char *condition) {
   int fh, i, length;
   char str[NAME_LENGTH], av_encoded[NAME_LENGTH], *buf, *buf2, *p1, *p2, *p3;

   fh = open(config_file, O_RDWR | O_BINARY, 0644);
   if (fh < 0) {
      sprintf(str, loc("Cannot open file <b>%s</b>"), config_file);
      strcat(str, ": ");
      strcat(str, strerror(errno));
      show_error(str);
      return 0;
   }

   /* do not allow HTML code in value */
   strencode2(av_encoded, attrvalue, sizeof(av_encoded));

   /* read previous contents */
   length = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   buf = xmalloc(length + strlen(av_encoded) + 3);
   read(fh, buf, length);
   buf[length] = 0;

   /* find location of options */
   if (condition && condition[0])
      set_condition(condition);
   else
      set_condition("");

   sprintf(str, "Options %s", attrname);
   p1 = (char *) find_param(buf, lbs->name, str);
   if (p1 == NULL) {
      sprintf(str, "MOptions %s", attrname);
      p1 = (char *) find_param(buf, lbs->name, str);
   }
   if (p1 == NULL) {
      sprintf(str, "ROptions %s", attrname);
      p1 = (char *) find_param(buf, lbs->name, str);
   }
   if (p1 == NULL)
      return 0;

   p2 = strchr(p1, '\n');
   if (p2 && *(p2 - 1) == '\r')
      p2--;

   /* save tail */
   buf2 = NULL;
   if (p2)
      buf2 = xstrdup(p2);

   /* add option */
   p3 = strchr(p1, '\n');
   if (p3 == NULL)
      p3 = p1 + strlen(p1);
   while (*(p3 - 1) == '\n' || *(p3 - 1) == '\r' || *(p3 - 1) == ' ' || *(p3 - 1) == '\t')
      p3--;

   sprintf(p3, ", %s", av_encoded);
   if (p2) {
      strlcat(buf, buf2, length + strlen(av_encoded) + 3);
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