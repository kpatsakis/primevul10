int build_subst_list(LOGBOOK *lbs, char list[][NAME_LENGTH], char value[][NAME_LENGTH],
                     char attrib[][NAME_LENGTH], BOOL format_date) {
   int i;
   char str[NAME_LENGTH], format[256], full_name[256], user_email[256];
   time_t t;
   struct tm *ts;

   /* copy attribute list */
   i = 0;
   if (attrib != NULL)
      for (; i < lbs->n_attr; i++) {
         strcpy(list[i], attr_list[i]);
         if (attrib) {
            if ((attr_flags[i] & AF_DATE) && format_date) {

               t = (time_t) atoi(attrib[i]);
               ts = localtime(&t);
               assert(ts);

               sprintf(str, "Date format %s", attr_list[i]);
               if (!getcfg(lbs->name, str, format, sizeof(format)))
                  if (!getcfg(lbs->name, "Date format", format, sizeof(format)))
                     strcpy(format, DEFAULT_DATE_FORMAT);

               my_strftime(value[i], NAME_LENGTH, format, ts);

            } else if ((attr_flags[i] & AF_DATETIME) && format_date) {

               t = (time_t) atoi(attrib[i]);
               ts = localtime(&t);
               assert(ts);

               sprintf(str, "Time format %s", attr_list[i]);
               if (!getcfg(lbs->name, str, format, sizeof(format)))
                  if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
                     strcpy(format, DEFAULT_TIME_FORMAT);

               my_strftime(value[i], NAME_LENGTH, format, ts);

            } else
               strcpy(value[i], attrib[i]);
         } else
            strlcpy(value[i], isparam(attr_list[i]) ? getparam(attr_list[i]) : "", NAME_LENGTH);
      }

   /* add remote host */
   strcpy(list[i], "remote_host");
   strlcpy(value[i++], rem_host, NAME_LENGTH);

   /* add local host */
   strcpy(list[i], "host");
   strlcpy(value[i++], host_name, NAME_LENGTH);

   /* add forwarded user */
   strcpy(list[i], "http_user");
   strlcpy(value[i++], http_user, NAME_LENGTH);

   /* add user names */
   strcpy(list[i], "short_name");
   if (isparam("unm")) {
      strlcpy(value[i++], getparam("unm"), NAME_LENGTH);
      get_user_line(lbs, getparam("unm"), NULL, full_name, user_email, NULL, NULL, NULL);
   } else {
      strlcpy(value[i++], loc("Anonymous"), NAME_LENGTH);
      strcpy(full_name, loc("Anonymous"));
      user_email[0] = 0;
   }

   strcpy(list[i], "long_name");
   strlcpy(value[i++], full_name, NAME_LENGTH);

   /* add email */
   if (user_email[0]) {
      strcpy(list[i], "user_email");
      strcpy(value[i], "mailto:");
      strlcat(value[i++], user_email, NAME_LENGTH);
   }

   /* add logbook */
   if (lbs) {
      strcpy(list[i], "logbook");
      strlcpy(value[i++], lbs->name, NAME_LENGTH);

      /* add logbook */
      strcpy(list[i], "elogbook");
      strlcpy(value[i++], lbs->name_enc, NAME_LENGTH);
   }

   /* add date */
   strcpy(list[i], "date");
   time(&t);
   if (format_date) {
      ts = localtime(&t);
      assert(ts);
      if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
         strcpy(format, DEFAULT_TIME_FORMAT);

      my_strftime(str, sizeof(str), format, ts);
   } else
      sprintf(str, "%d", (int) t);
   strcpy(value[i++], str);

   /* add UTC date */
   strcpy(list[i], "utcdate");
   time(&t);
   if (format_date) {
      ts = gmtime(&t);
      if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
         strcpy(format, DEFAULT_TIME_FORMAT);

      my_strftime(str, sizeof(str), format, ts);
   } else
      sprintf(str, "%d", (int) t);
   strcpy(value[i++], str);

   /* add ELOG version and revision */
   strcpy(list[i], "version");
   strcpy(value[i++], VERSION);

   strcpy(list[i], "revision");
   sprintf(value[i++], "%s", git_revision());

   return i;
}