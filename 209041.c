int sid_check(char *sid, char *user_name) {
   int i;
   time_t now;

   if (sid == NULL)
      return FALSE;

   time(&now);
   for (i = 0; i < _n_sid; i++) {
      if (strcmp(_sid[i].host_ip, (char *) inet_ntoa(rem_addr)) == 0 && strcmp(_sid[i].session_id, sid) == 0) {
         strcpy(user_name, _sid[i].user_name);
         _sid[i].time = now;
         return TRUE;
      }
   }

   return FALSE;
}