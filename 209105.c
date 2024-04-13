time_t convert_datetime(char *date_string) {
   /* convert date string in MM/DD/YY h:m:s AM/PM or DD.MM.YY hh:m:s format into Unix time */
   int year, month, day, hour, min = 0, sec = 0;
   char *p, str[256];
   struct tm tms;
   time_t ltime;

   strlcpy(str, date_string, sizeof(str));
   month = day = year = 0;

   if (strchr(str, '[')) {
      ltime = atoi(strchr(str, '[') + 1);
      return ltime;
   } else if (strchr(str, '/')) {
      /* MM/DD/YY format */
      p = strtok(str, "/");
      if (p) {
         month = atoi(p);
         p = strtok(NULL, "/");
         if (p) {
            day = atoi(p);
            p = strtok(NULL, "/");
            if (p)
               year = atoi(p);
         }
      }
   } else if (strchr(str, '.')) {
      /* DD.MM.YY format */
      p = strtok(str, ".");
      if (p) {
         day = atoi(p);
         p = strtok(NULL, ".");
         if (p) {
            month = atoi(p);
            p = strtok(NULL, ".");
            if (p)
               year = atoi(p);
         }
      }
   } else
      return 0;

   if (!strchr(p, ' '))
      return 0;
   p = strchr(p, ' ') + 1;

   strlcpy(str, p, sizeof(str));
   p = strtok(str, ":");
   if (p) {
      hour = atoi(p);
      p = strtok(NULL, ":");
      if (p) {
         min = atoi(p);
         p = strtok(NULL, ":");
         if (p)
            sec = atoi(p);
      }
   } else
      return 0;

   if (stristr(p, "PM") && hour < 12)
      hour += 12;

   /* calculate years */
   if (year > 1900)             /* 1900-2100 */
      year += 0;
   else if (year < 70)          /* 00-69 */
      year += 2000;
   else if (year < 100)         /* 70-99 */
      year += 1900;

   /* use last day of month */
   memset(&tms, 0, sizeof(struct tm));
   tms.tm_year = year - 1900;
   tms.tm_mon = month - 1;
   tms.tm_mday = day;
   tms.tm_hour = hour;
   tms.tm_min = min;
   tms.tm_sec = sec;

   ltime = mktime(&tms);

   return ltime;
}