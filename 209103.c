int el_move_message(LOGBOOK *lbs, int old_id, int new_id) {
   int status, size;
   char date[80], attrib[MAX_N_ATTR][NAME_LENGTH], *text, in_reply_to[80], reply_to[MAX_REPLY_TO * 10],
           encoding[80], locked_by[256], draft[256], att_file[MAX_ATTACHMENTS][256];

   /* retrieve message */
   text = xmalloc(TEXT_SIZE);
   size = TEXT_SIZE;
   status = el_retrieve(lbs, old_id, date, attr_list, attrib, lbs->n_attr, text, &size, in_reply_to,
                        reply_to, att_file, encoding, locked_by, draft);
   if (status != EL_SUCCESS)
      return 0;

   /* submit as new message */
   status = el_submit(lbs, new_id, FALSE, date, attr_list, attrib, lbs->n_attr, text, in_reply_to, reply_to,
                      encoding, att_file, FALSE, locked_by, draft);

   xfree(text);

   if (status != new_id)
      return 0;

   /* correct links */
   el_correct_links(lbs, old_id, new_id);

   /* delete original message */
   el_delete_message(lbs, old_id, FALSE, NULL, FALSE, FALSE);

   return 1;
}