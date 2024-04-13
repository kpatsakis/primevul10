void attrib_from_param(int n_attr, char attrib[MAX_N_ATTR][NAME_LENGTH]) {
   int i, j, first, year, month, day, hour, min, sec;
   char str[NAME_LENGTH], ua[NAME_LENGTH];
   time_t ltime;
   struct tm ts;

   for (i = 0; i < n_attr; i++) {
      strcpy(ua, attr_list[i]);
      stou(ua);
      if (attr_flags[i] & (AF_MULTI | AF_MUSERLIST | AF_MUSEREMAIL)) {
         attrib[i][0] = 0;
         first = 1;
         for (j = 0; j < MAX_N_LIST; j++) {
            sprintf(str, "%s_%d", ua, j);
            if (isparam(str)) {
               if (first)
                  first = 0;
               else
                  strlcat(attrib[i], " | ", NAME_LENGTH);
               if (strlen(attrib[i]) + strlen(getparam(str)) < NAME_LENGTH - 2)
                  strlcat(attrib[i], getparam(str), NAME_LENGTH);
               else
                  break;
            }
         }
      } else if (attr_flags[i] & AF_DATE) {

         if (isparam(ua))
            strlcpy(attrib[i], getparam(ua), NAME_LENGTH);
         else {
            sprintf(str, "y%d", i);
            year = atoi(isparam(str) ? getparam(str) : "");
            if (year < 100)
               year += 2000;

            sprintf(str, "m%d", i);
            month = atoi(isparam(str) ? getparam(str) : "");

            sprintf(str, "d%d", i);
            day = atoi(isparam(str) ? getparam(str) : "");

            memset(&ts, 0, sizeof(struct tm));
            ts.tm_year = year - 1900;
            ts.tm_mon = month - 1;
            ts.tm_mday = day;
            ts.tm_hour = 12;

            if (month && day) {
               ltime = mktime(&ts);
               sprintf(attrib[i], "%d", (int) ltime);
            } else
               strcpy(attrib[i], "");
         }

      } else if (attr_flags[i] & AF_DATETIME) {

         if (isparam(ua))
            strlcpy(attrib[i], getparam(ua), NAME_LENGTH);
         else {
            sprintf(str, "y%d", i);
            year = atoi(isparam(str) ? getparam(str) : "");
            if (year < 100)
               year += 2000;

            sprintf(str, "m%d", i);
            month = atoi(isparam(str) ? getparam(str) : "");

            sprintf(str, "d%d", i);
            day = atoi(isparam(str) ? getparam(str) : "");

            sprintf(str, "h%d", i);
            hour = atoi(isparam(str) ? getparam(str) : "");

            sprintf(str, "n%d", i);
            min = atoi(isparam(str) ? getparam(str) : "");

            sprintf(str, "c%d", i);
            sec = atoi(isparam(str) ? getparam(str) : "");

            memset(&ts, 0, sizeof(struct tm));
            ts.tm_year = year - 1900;
            ts.tm_mon = month - 1;
            ts.tm_mday = day;
            ts.tm_hour = hour;
            ts.tm_min = min;
            ts.tm_sec = sec;
            ts.tm_isdst = -1;

            if (month && day) {
               ltime = mktime(&ts);
               sprintf(attrib[i], "%d", (int) ltime);
            } else
               strcpy(attrib[i], "");
         }

      } else {
         if (isparam(attr_list[i]))
            strlcpy(attrib[i], getparam(attr_list[i]), NAME_LENGTH);
         else if (isparam(ua))
            strlcpy(attrib[i], getparam(ua), NAME_LENGTH);
         else
            attrib[i][0] = 0;
      }
   }
}