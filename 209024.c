time_t search_last_reply(LOGBOOK *lbs, int *message_id) {
   char reply_to[MAX_REPLY_TO * 10], date[80];
   int n_reply, i, id;
   char *list;
   time_t lt, last;

   list = (char *) xmalloc(MAX_REPLY_TO * NAME_LENGTH);

   el_retrieve(lbs, *message_id, date, NULL, NULL, 0, NULL, 0, NULL, reply_to, NULL, NULL, NULL, NULL);
   lt = date_to_ltime(date);

   /* if no reply, this is the last message in thread */
   if (reply_to[0] == 0) {
      xfree(list);
      return lt;
   }

   n_reply = strbreak(reply_to, (char (*)[NAME_LENGTH]) list, MAX_REPLY_TO, ",", FALSE);
   last = lt;
   for (i = 0; i < n_reply; i++) {
      id = atoi(list + i * NAME_LENGTH);
      lt = search_last_reply(lbs, &id);
      if (lt > last) {
         last = lt;
         *message_id = id;
      }
   }

   xfree(list);

   return last;
}