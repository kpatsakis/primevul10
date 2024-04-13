time_t retrieve_date(char *index, BOOL bstart) {
   int year, month, day, hour, min, sec, current_year, current_month;
   char pm[10], py[10], pd[10], ph[10], pn[10], ps[10], str[NAME_LENGTH], str2[NAME_LENGTH];
   struct tm tms;
   time_t ltime;

   sprintf(pm, "m%s", index);
   sprintf(py, "y%s", index);
   sprintf(pd, "d%s", index);
   sprintf(ph, "h%s", index);
   sprintf(pn, "n%s", index);
   sprintf(ps, "c%s", index);

   time(&ltime);
   memcpy(&tms, localtime(&ltime), sizeof(tms));
   current_year = tms.tm_year + 1900;
   current_month = tms.tm_mon + 1;

   if (!isparam(pm) && !isparam(py) && !isparam(pd))
      return 0;

   /* if year not given, use current year */
   if (!isparam(py))
      year = current_year;
   else
      year = atoi(getparam(py));
   if (year < 1970) {
      sprintf(str, "Error: Year %s out of range", getparam(py));
      strencode2(str2, str, sizeof(str2));
      show_error(str2);
      return -1;
   }

   /* if month not given, use current month */
   if (isparam(pm)) {
      month = atoi(getparam(pm));
   } else
      month = current_month;

   if (isparam(pd))
      day = atoi(getparam(pd));
   else {
      /* if day not given, use 1 if start date */
      if (bstart)
         day = 1;
      else {
         /* use last day of month */
         memset(&tms, 0, sizeof(struct tm));
         tms.tm_year = year - 1900;
         tms.tm_mon = month - 1 + 1;
         tms.tm_mday = 1;
         tms.tm_hour = 12;

         if (tms.tm_year < 90)
            tms.tm_year += 100;
         ltime = mktime(&tms);
         ltime -= 3600 * 24;
         memcpy(&tms, localtime(&ltime), sizeof(struct tm));
         day = tms.tm_mday;
      }

   }

   /* if hour not given, use 0 */
   if (isparam(ph)) {
      hour = atoi(getparam(ph));
   } else
      hour = 0;

   /* if minute not given, use 0 */
   if (isparam(pn)) {
      min = atoi(getparam(pn));
   } else
      min = 0;

   /* if second not given, use 0 */
   if (isparam(ps)) {
      sec = atoi(getparam(ps));
   } else
      sec = 0;

   memset(&tms, 0, sizeof(struct tm));
   tms.tm_year = year - 1900;
   tms.tm_mon = month - 1;
   tms.tm_mday = day;
   tms.tm_hour = hour;
   tms.tm_min = min;
   tms.tm_sec = sec;
   tms.tm_isdst = -1;

   if (tms.tm_year < 90)
      tms.tm_year += 100;

   ltime = mktime(&tms);

   if (!bstart && isparam(ph) == 0)
      /* end time is first second of next day */
      ltime += 3600 * 24;

   return ltime;
}