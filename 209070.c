int sid_remove(char *sid) {
   int i;

   if (sid == NULL)
      return FALSE;

   for (i = 0; i < _n_sid; i++) {
      if (strcmp(_sid[i].session_id, sid) == 0) {
         memset(&_sid[i], 0, sizeof(SESSION_ID));
         return TRUE;
      }
   }

   return FALSE;
}