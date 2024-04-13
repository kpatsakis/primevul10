void set_cookie(LOGBOOK *lbs, char *name, char *value, BOOL global, char *expiration) {
   char lb_name[256], str[NAME_LENGTH], format[80];
   double exp;
   time_t now;
   struct tm *gmt;

   if (lbs)
      strcpy(lb_name, lbs->name);
   else
      strcpy(lb_name, "global");

   if (value != NULL)
      rsprintf("Set-Cookie: %s=%s;", name, value);
   else
      rsprintf("Set-Cookie: %s;", name);

   /* add path */
   if (global) {
      /* path for all logbooks */
      if (getcfg(lb_name, "URL", str, sizeof(str))) {
         extract_path(str);
         url_encode(str, sizeof(str));
         rsprintf(" path=/%s;", str);
      } else
         rsprintf(" path=/;");
   } else {
      /* path for individual logbook */
      if (getcfg(lb_name, "URL", str, sizeof(str))) {
         extract_path(str);
         url_encode(str, sizeof(str));
         if (str[0])
            rsprintf(" path=/%s/%s;", str, lbs->name_enc);
         else
            rsprintf(" path=/%s;", lbs->name_enc);
      } else
         rsprintf(" path=/%s;", lbs->name_enc);
   }

   exp = atof(expiration);

   /* to clear a cookie, set expiration date to yesterday */
   if (value != NULL && value[0] == 0)
      exp = -24;

   /* add expriation date */
   if (exp != 0 && exp < 100000) {
      time(&now);
      now += (int) (3600 * exp);
      gmt = gmtime(&now);
      strcpy(format, "%A, %d-%b-%y %H:%M:%S GMT");
      strftime(str, sizeof(str), format, gmt);

      rsprintf(" expires=%s;", str);
   }

   rsprintf("\r\n");
}