void get_rfc2822_date(char *date, int size, time_t ltime) {
   time_t now;
   char buf[256];
   int offset;
   struct tm *ts;

   /* switch locale temporarily back to english to comply with RFC2822 date format */
   setlocale(LC_ALL, "C");

   if (ltime == 0)
      time(&now);
   else
      now = ltime;
   ts = localtime(&now);
   assert(ts);
   strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", ts);
   offset = (-(int) my_timezone());
   if (ts->tm_isdst)
      offset += 3600;
   snprintf(date, size - 1, "%s %+03d%02d", buf, (int) (offset / 3600),
            (int) ((abs((int) offset) / 60) % 60));
}