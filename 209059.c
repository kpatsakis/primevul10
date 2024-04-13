void remove_reference(LOGBOOK *lbs, int message_id, int remove_id, BOOL reply_to_flag) {
   char date[80], attr[MAX_N_ATTR][NAME_LENGTH], enc[80], in_reply_to[80], reply_to[MAX_REPLY_TO * 10],
           att[MAX_ATTACHMENTS][256], lock[256], draft[256], *p, *ps, *message;
   int size, status;

   /* retrieve original message */
   size = TEXT_SIZE + 1000;
   message = (char *) xmalloc(size);
   status = el_retrieve(lbs, message_id, date, attr_list, attr, lbs->n_attr, message, &size, in_reply_to,
                        reply_to, att, enc, lock, draft);
   if (status != EL_SUCCESS)
      return;

   if (reply_to_flag)
      p = reply_to;
   else
      p = in_reply_to;

   while (*p) {
      while (*p && (*p == ',' || *p == ' '))
         p++;

      ps = p;
      while (isdigit(*ps))
         ps++;

      while (*ps && (*ps == ',' || *ps == ' '))
         ps++;

      if (atoi(p) == remove_id)
         strcpy(p, ps);
      else
         while (isdigit(*p))
            p++;
   }

   /* write modified message */
   el_submit(lbs, message_id, TRUE, date, attr_list, attr, lbs->n_attr, message, in_reply_to, reply_to, enc,
             att, TRUE, lock, NULL);

   xfree(message);
}