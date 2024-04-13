int submit_elog_reply(LOGBOOK *lbs, int message_id, char attrib[MAX_N_ATTR][NAME_LENGTH], char *text) {
   int n_reply, i, status;
   char str1[80], str2[80], att_file[MAX_ATTACHMENTS][256], reply_to[MAX_REPLY_TO * 10],
           list[MAX_N_ATTR][NAME_LENGTH];

   status = el_retrieve(lbs, message_id, NULL, attr_list, NULL, 0,
                        NULL, NULL, NULL, reply_to, att_file, NULL, NULL, NULL);
   if (status != EL_SUCCESS)
      return status;

   sprintf(str1, "- %s -", loc("keep original text"));
   sprintf(str2, "<p>- %s -</p>", loc("keep original text"));
   if (strcmp(text, str1) == 0 || strcmp(text, str2) == 0)
      message_id = el_submit(lbs, message_id, TRUE, "<keep>", attr_list, attrib, lbs->n_attr, "<keep>",
                             "<keep>", "<keep>", "<keep>", att_file, TRUE, NULL, NULL);
   else
      message_id = el_submit(lbs, message_id, TRUE, "<keep>", attr_list, attrib, lbs->n_attr, text, "<keep>",
                             "<keep>", "<keep>", att_file, TRUE, NULL, NULL);

   if (message_id < 0)
      return 0;

   if (isparam("elmode") && strieq(getparam("elmode"), "threaded")) {

      // go through all replies in threaded mode
      n_reply = strbreak(reply_to, list, MAX_N_ATTR, ",", FALSE);
      for (i = 0; i < n_reply; i++) {
         submit_elog_reply(lbs, atoi(list[i]), attrib, text);
      }
   }

   return EL_SUCCESS;
}