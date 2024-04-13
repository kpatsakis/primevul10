int receive_message(LOGBOOK *lbs, char *url, int message_id, char *error_str, BOOL bnew) {
   int i, status, size, n_attr, header_size, ssl;
   char str[NAME_LENGTH], str2[NAME_LENGTH], *p, *p2, *message, date[80], attrib[MAX_N_ATTR][NAME_LENGTH],
           in_reply_to[80], reply_to[MAX_REPLY_TO * 10], encoding[80], locked_by[256],
           attachment[MAX_ATTACHMENTS][MAX_PATH_LENGTH], attachment_all[64 * MAX_ATTACHMENTS];

   error_str[0] = 0;

   combine_url(lbs, url, "", str, sizeof(str), &ssl);
   sprintf(str + strlen(str), "%d?cmd=%s", message_id, loc("Download"));

   retrieve_url(lbs, str, ssl, &message, TRUE);
   if (message == NULL) {
      sprintf(error_str, loc("Cannot receive \"%s\""), str);
      return -1;
   }
   p = strstr(message, "\r\n\r\n");
   if (p == NULL) {
      if (isparam("debug"))
         rsputs(message);
      xfree(message);
      sprintf(error_str, loc("Cannot receive \"%s\""), str);
      return -1;
   }
   p += 4;

   /* check for correct ID */
   if (atoi(p + 8) != message_id) {
      if (isparam("debug"))
         rsputs(message);
      sprintf(error_str, loc("Received wrong entry id \"%d\""), atoi(p + 8));
      xfree(message);
      return -1;
   }

   /* decode entry */
   el_decode(p, "Date: ", date, sizeof(date));
   el_decode_intlist(p, "Reply to: ", reply_to, sizeof(reply_to));
   el_decode_int(p, "In reply to: ", in_reply_to, sizeof(in_reply_to));

   /* derive attribute names from message */
   for (i = 0;; i++) {
      el_enum_attr(p, i, attr_list[i], attrib[i]);
      if (!attr_list[i][0])
         break;
   }
   n_attr = i;

   el_decode(p, "Attachment: ", attachment_all, sizeof(attachment_all));
   el_decode(p, "Encoding: ", encoding, sizeof(encoding));

   /* break apart attachements */
   for (i = 0; i < MAX_ATTACHMENTS; i++)
      attachment[i][0] = 0;

   for (i = 0; i < MAX_ATTACHMENTS; i++) {
      if (i == 0)
         p2 = strtok(attachment_all, ",");
      else
         p2 = strtok(NULL, ",");

      if (p2 != NULL)
         strcpy(attachment[i], p2);
      else
         break;
   }

   el_decode(p, "Locked by: ", locked_by, sizeof(locked_by));
   if (locked_by[0]) {
      xfree(message);
      sprintf(error_str, loc("Entry #%d is locked on remote server"), message_id);
      return -1;
   }

   p = strstr(message, "========================================\n");

   /* check for \n -> \r conversion (e.g. zipping/unzipping) */
   if (p == NULL)
      p = strstr(message, "========================================\r");

   if (p != NULL) {
      p += 41;

      /* remove last CR */
      if (p[strlen(p) - 1] == '\n')
         p[strlen(p) - 1] = 0;

      status = el_submit(lbs, message_id, !bnew, date, attr_list, attrib, n_attr, p, in_reply_to, reply_to,
                         encoding, attachment, FALSE, "", NULL);

      xfree(message);

      if (status != message_id) {
         strlcpy(error_str, loc("Cannot save remote entry locally"), 256);
         return -1;
      }

      for (i = 0; i < MAX_ATTACHMENTS; i++) {
         if (attachment[i][0]) {

            combine_url(lbs, url, "", str, sizeof(str), &ssl);
            strlcpy(str2, attachment[i], sizeof(str2));
            str2[13] = '/';
            strlcat(str, str2, sizeof(str));

            size = retrieve_url(lbs, str, ssl, &message, TRUE);
            p = strstr(message, "\r\n\r\n");
            if (p == NULL) {
               xfree(message);
               sprintf(error_str, loc("Cannot receive \"%s\""), str);
               return -1;
            }
            p += 4;
            header_size = p - message;

            el_submit_attachment(lbs, attachment[i], p, size - header_size, NULL);
            xfree(message);
         }
      }
   } else {
      xfree(message);
      return -1;
   }

   return 1;
}