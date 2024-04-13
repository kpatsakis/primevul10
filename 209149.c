void submit_elog_mirror(LOGBOOK *lbs) {
   char str[1000], date[80], attrib_value[MAX_N_ATTR][NAME_LENGTH], attrib_name[MAX_N_ATTR][NAME_LENGTH],
           in_reply_to[80], encoding[80], reply_to[MAX_REPLY_TO * 10], att_file[MAX_ATTACHMENTS][256],
           name[NAME_LENGTH], value[NAME_LENGTH];
   int i, message_id, n_attr;
   BOOL bedit;

   /* get attachments */
   for (i = 0; i < MAX_ATTACHMENTS; i++) {
      sprintf(str, "attachment%d", i);
      strcpy(att_file[i], isparam(str) ? getparam(str) : "");
   }

   reply_to[0] = 0;
   in_reply_to[0] = 0;
   date[0] = 0;
   encoding[0] = 0;
   bedit = FALSE;
   n_attr = 0;
   message_id = 0;

   /* retrieve attributes */
   for (i = 0; i < MAX_PARAM; i++) {
      if (enumparam(i, name, value)) {

         if (strieq(name, "mirror_id"))
            message_id = atoi(value);
         else if (strieq(name, "entry_date"))
            strlcpy(date, value, sizeof(date));
         else if (strieq(name, "reply_to"))
            strlcpy(reply_to, value, sizeof(reply_to));
         else if (strieq(name, "in_reply_to"))
            strlcpy(in_reply_to, value, sizeof(in_reply_to));
         else if (strieq(name, "encoding"))
            strlcpy(encoding, value, sizeof(encoding));
         else if (!strieq(name, "cmd") && !strieq(name, "full_name") && !strieq(name, "user_email")
                  && !strieq(name, "unm") && !strieq(name, "upwd") && !strieq(name, "wpwd") && strncmp(name,
                                                                                                       "attachment",
                                                                                                       10) !=
                                                                                               0) {
            strlcpy(attrib_name[n_attr], name, NAME_LENGTH);
            strlcpy(attrib_value[n_attr++], value, NAME_LENGTH);
         }
      } else
         break;
   }

   if (message_id == 0 || date[0] == 0) {
      show_error(loc("Invalid mirror_id or entry_date"));
      return;
   }

   /* check if message already exists */
   for (i = 0; i < *lbs->n_el_index; i++)
      if (lbs->el_index[i].message_id == message_id) {
         bedit = TRUE;
         break;
      }

   message_id = el_submit(lbs, message_id, bedit, date, attrib_name, attrib_value, n_attr, getparam("text"),
                          in_reply_to, reply_to, encoding, att_file, FALSE, NULL, NULL);

   if (message_id <= 0) {
      sprintf(str, loc("New entry cannot be written to directory \"%s\""), lbs->data_dir);
      strcat(str, "\n<p>");
      strcat(str, loc("Please check that it exists and elogd has write access"));
      show_error(str);
      return;
   }

   sprintf(str, "%d", message_id);
   redirect(lbs, str);
}