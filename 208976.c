time_t date_to_ltime(char *date) {
   struct tm tms;
   int i, date_zone, local_zone;
   time_t ltime;

   memset(&tms, 0, sizeof(struct tm));

   if (strlen(date) > 25) {

      /* RFC2822 compliant date */
      for (i = 0; i < 12; i++)
         if (strncmp(date + 8, mname[i], 3) == 0)
            break;
      tms.tm_mon = i;

      tms.tm_mday = atoi(date + 5);
      tms.tm_hour = atoi(date + 17);
      tms.tm_min = atoi(date + 20);
      tms.tm_sec = atoi(date + 23);
      tms.tm_year = atoi(date + 12) - 1900;
      tms.tm_isdst = -1;

      if (tms.tm_year < 90)
         tms.tm_year += 100;

      ltime = mktime(&tms);

      /* correct for difference between local time zone (used by mktime) and time zone of date */
      date_zone = atoi(date + 26);

      /* correct for incorrect date_zone */
      if (date_zone > 2400 || date_zone < -2400)
         date_zone = 0;
      date_zone = (abs(date_zone) % 100) * 60 + (date_zone) / 100 * 3600;

      local_zone = my_timezone();
      if (tms.tm_isdst)
         local_zone -= 3600;

      ltime = ltime - local_zone - date_zone;

   } else {

      /* ctime() complient date */
      for (i = 0; i < 12; i++)
         if (strncmp(date + 4, mname[i], 3) == 0)
            break;
      tms.tm_mon = i;

      tms.tm_mday = atoi(date + 8);
      tms.tm_hour = atoi(date + 11);
      tms.tm_min = atoi(date + 14);
      tms.tm_sec = atoi(date + 17);
      tms.tm_year = atoi(date + 20) - 1900;
      tms.tm_isdst = -1;

      if (tms.tm_year < 90)
         tms.tm_year += 100;

      ltime = mktime(&tms);
   }

   return ltime;
}