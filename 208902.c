void add_subst_time(LOGBOOK *lbs, char list[][NAME_LENGTH], char value[][NAME_LENGTH], char *item,
                    char *date, int *i, int flags) {
   char format[80], str[256];
   time_t ltime;
   struct tm *pts;

   if (flags & (AF_DATE | AF_DATETIME)) {
      ltime = date_to_ltime(date);
      sprintf(str, "%d", (int) ltime);
      add_subst_list(list, value, item, str, i);
   } else {
      if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
         strcpy(format, DEFAULT_TIME_FORMAT);
      ltime = date_to_ltime(date);
      pts = localtime(&ltime);
      assert(pts);
      my_strftime(str, sizeof(str), format, pts);

      add_subst_list(list, value, item, str, i);
   }
}