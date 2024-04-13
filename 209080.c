int sid_new(LOGBOOK *lbs, const char *user, const char *host, char *sid) {
   double exp;
   time_t now;
   int i, new_i;
   char str[256];

   time(&now);
   new_i = 0;

   if (_sid == NULL) {
      _sid = (SESSION_ID *) malloc(sizeof(SESSION_ID));
      _n_sid = 1;
      new_i = 0;
   } else {

      exp = 24;
      str[0] = 0;
      if (lbs == NULL)
         getcfg("global", "Login expiration", str, sizeof(str));
      else
         getcfg(lbs->name, "Login expiration", str, sizeof(str));

      if (atof(str) > 0)
         exp = atof(str);
      if (exp < 24)
         exp = 24;              /* one day minimum for dangling edit pages */

      /* search for expired sid */
      for (i = 0; i < _n_sid; i++) {
         if (_sid[i].time + exp * 3600 < now) {
            new_i = i;
            break;
         }
      }

      if (i == _n_sid) {
         _sid = (SESSION_ID *) realloc(_sid, sizeof(SESSION_ID) * (_n_sid + 1));
         new_i = _n_sid;
         _n_sid++;
      }
   }

   strlcpy(_sid[new_i].user_name, user, sizeof(_sid[0].user_name));
   strlcpy(_sid[new_i].host_ip, host, sizeof(_sid[0].host_ip));
   for (i = 0; i < 4; i++)
      sprintf(sid + i * 4, "%04X", rand() % 0x10000);
   sid[16] = 0;
   strlcpy(_sid[new_i].session_id, sid, sizeof(_sid[0].session_id));
   _sid[new_i].time = now;

   return 1;
}