int el_draft_message(LOGBOOK *lbs, int message_id, char *user, BOOL bdraft)
/* lock message for editing */
{
   int size;
   char date[80], attrib[MAX_N_ATTR][NAME_LENGTH], text[TEXT_SIZE], in_reply_to[80],
           reply_to[MAX_REPLY_TO * 10], encoding[80], locked_by[256], draft[256];
   char att_file[MAX_ATTACHMENTS][256];

   /* retrieve message */
   size = sizeof(text);
   el_retrieve(lbs, message_id, date, attr_list, attrib, lbs->n_attr, text, &size, in_reply_to, reply_to,
               att_file, encoding, locked_by, draft);

   /* submit message, undraft if bdraft == FALSE */
   el_submit(lbs, message_id, TRUE, date, attr_list, attrib, lbs->n_attr, text, in_reply_to, reply_to,
             encoding, att_file, FALSE, locked_by, bdraft ? user : NULL);

   return EL_SUCCESS;
}