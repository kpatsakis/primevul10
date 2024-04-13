int find_thread_head(LOGBOOK *lbs, int message_id) {
   int i;

   /* search index of message */
   for (i = 0; i < *lbs->n_el_index; i++)
      if (lbs->el_index[i].message_id == message_id)
         break;

   if (lbs->el_index[i].in_reply_to)
      return find_thread_head(lbs, lbs->el_index[i].in_reply_to);

   return message_id;
}