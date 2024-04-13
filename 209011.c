int el_move_message_thread(LOGBOOK *lbs, int message_id) {
   int i, n, size, new_id;
   char date[80], attrib[MAX_N_ATTR][NAME_LENGTH], *text, in_reply_to[80], reply_to[MAX_REPLY_TO * 10],
           encoding[80], locked_by[256], draft[256];
   char list[MAX_N_ATTR][NAME_LENGTH], str[256];
   char att_file[MAX_ATTACHMENTS][256];

   /* retrieve message */
   text = xmalloc(TEXT_SIZE);
   size = TEXT_SIZE;
   el_retrieve(lbs, message_id, date, attr_list, attrib, lbs->n_attr, text, &size, in_reply_to, reply_to,
               att_file, encoding, locked_by, draft);

   /* submit as new message */
   date[0] = 0;
   new_id = el_submit(lbs, 0, FALSE, date, attr_list, attrib, lbs->n_attr, text, in_reply_to, reply_to,
                      encoding, att_file, FALSE, locked_by, draft);

   xfree(text);

   /* correct links */
   el_correct_links(lbs, message_id, new_id);

   /* delete original message */
   el_delete_message(lbs, message_id, FALSE, NULL, FALSE, FALSE);

   /* move all replies recursively */
   if (getcfg(lbs->name, "Resubmit replies", str, sizeof(str)) && atoi(str) == 1) {
      if (reply_to[0]) {
         n = strbreak(reply_to, list, MAX_N_ATTR, ",", FALSE);
         for (i = 0; i < n; i++)
            el_move_message_thread(lbs, atoi(list[i]));
      }
   }

   return new_id;
}