int el_correct_links(LOGBOOK *lbs, int old_id, int new_id)
/* If a message gets resubmitted, the links to that message are wrong.
 This routine corrects that. */
{
   int i, i1, n, n1, size;
   char date[80], *attrib, *text, in_reply_to[80], reply_to[MAX_REPLY_TO * 10], encoding[80],
           locked_by[256], draft[256];
   char list[MAX_N_ATTR][NAME_LENGTH], list1[MAX_N_ATTR][NAME_LENGTH];
   char *att_file;

   attrib = (char *) xmalloc(MAX_N_ATTR * NAME_LENGTH);
   text = (char *) xmalloc(TEXT_SIZE);
   att_file = (char *) xmalloc(MAX_ATTACHMENTS * 256);

   el_retrieve(lbs, new_id, date, attr_list, (char (*)[1500]) attrib, lbs->n_attr, NULL, 0, in_reply_to,
               reply_to, (char (*)[256]) att_file, encoding, locked_by, draft);

   /* go through in_reply_to list */
   n = strbreak(in_reply_to, list, MAX_N_ATTR, ",", FALSE);
   for (i = 0; i < n; i++) {
      size = TEXT_SIZE;
      el_retrieve(lbs, atoi(list[i]), date, attr_list, (char (*)[1500]) attrib, lbs->n_attr, text, &size,
                  in_reply_to, reply_to, (char (*)[256]) att_file, encoding, locked_by, draft);

      n1 = strbreak(reply_to, list1, MAX_N_ATTR, ",", FALSE);
      reply_to[0] = 0;
      for (i1 = 0; i1 < n1; i1++) {
         /* replace old ID by new ID */
         if (atoi(list1[i1]) == old_id)
            sprintf(reply_to + strlen(reply_to), "%d", new_id);
         else
            strcat(reply_to, list1[i1]);

         if (i1 < n1 - 1)
            strcat(reply_to, ", ");
      }

      el_submit(lbs, atoi(list[i]), TRUE, date, attr_list, (char (*)[1500]) attrib, lbs->n_attr, text,
                in_reply_to, reply_to, encoding, (char (*)[256]) att_file, TRUE, locked_by, draft);
   }

   el_retrieve(lbs, new_id, date, attr_list, (char (*)[1500]) attrib, lbs->n_attr, NULL, 0, in_reply_to,
               reply_to, (char (*)[256]) att_file, encoding, locked_by, draft);

   /* go through reply_to list */
   n = strbreak(reply_to, list, MAX_N_ATTR, ",", FALSE);
   for (i = 0; i < n; i++) {
      size = sizeof(text);
      el_retrieve(lbs, atoi(list[i]), date, attr_list, (char (*)[1500]) attrib, lbs->n_attr, text, &size,
                  in_reply_to, reply_to, (char (*)[256]) att_file, encoding, locked_by, draft);

      n1 = strbreak(in_reply_to, list1, MAX_N_ATTR, ",", FALSE);
      in_reply_to[0] = 0;
      for (i1 = 0; i1 < n1; i1++) {
         /* replace old ID by new ID */
         if (atoi(list1[i1]) == old_id)
            sprintf(in_reply_to + strlen(in_reply_to), "%d", new_id);
         else
            strcat(in_reply_to, list1[i1]);

         if (i1 < n1 - 1)
            strcat(in_reply_to, ", ");
      }

      el_submit(lbs, atoi(list[i]), TRUE, date, attr_list, (char (*)[1500]) attrib, lbs->n_attr, text,
                in_reply_to, reply_to, encoding, (char (*)[256]) att_file, TRUE, locked_by, draft);
   }

   xfree(text);
   xfree(attrib);
   xfree(att_file);

   return EL_SUCCESS;
}