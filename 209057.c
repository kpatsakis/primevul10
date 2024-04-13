int el_retrieve(LOGBOOK *lbs, int message_id, char *date, char attr_list[MAX_N_ATTR][NAME_LENGTH],
                char attrib[MAX_N_ATTR][NAME_LENGTH], int n_attr, char *text, int *textsize,
                char *in_reply_to, char *reply_to, char attachment[MAX_ATTACHMENTS][MAX_PATH_LENGTH],
                char *encoding, char *locked_by, char *draft)
/********************************************************************
 Routine: el_retrieve

 Purpose: Retrieve an ELog entry by its message tab

 Input:
 LOGBOOK lbs             Logbook structure
 int    message_id       Message ID to retrieve
 int    *size            Size of text buffer

 Output:
 char   *tag             tag of retrieved message
 char   *date            Date/time of message recording
 char   attr_list        Names of attributes
 char   attrib           Values of attributes
 int    n_attr           Number of attributes
 char   *text            Message text
 char   *in_reply_to     Original message if this one is a reply
 char   *reply_to        Replies for current message
 char   *attachment[]    File attachments
 char   *encoding        Encoding of message
 char   *locked_by       User/Host if locked for editing
 char   *draft           User who drafted that message
 int    *size            Actual message text size

 Function value:
 EL_SUCCESS              Successful completion
 EL_EMPTY                Logbook is empty
 EL_NO_MSG               Message doesn't exist
 EL_FILE_ERROR           Internal error

 \********************************************************************/
{
   int i, index, size, fh;
   char str[NAME_LENGTH], file_name[MAX_PATH_LENGTH * 3], *p;
   char *message, attachment_all[64 * MAX_ATTACHMENTS];

   if (message_id == 0)
      /* open most recent message */
      message_id = el_search_message(lbs, EL_LAST, 0, FALSE);

   if (message_id == 0)
      return EL_EMPTY;

   for (index = 0; index < *lbs->n_el_index; index++)
      if (lbs->el_index[index].message_id == message_id)
         break;

   if (index == *lbs->n_el_index)
      return EL_NO_MSG;

   snprintf(file_name, sizeof(file_name), "%s%s%s", lbs->data_dir, lbs->el_index[index].subdir,
            lbs->el_index[index].file_name);
   fh = open(file_name, O_RDONLY | O_BINARY, 0644);
   if (fh < 0) {
      /* file might have been deleted, rebuild index */
      el_build_index(lbs, TRUE);
      return el_retrieve(lbs, message_id, date, attr_list, attrib, n_attr, text, textsize, in_reply_to,
                         reply_to, attachment, encoding, locked_by, draft);
   }

   message = xmalloc(TEXT_SIZE + 1000);

   lseek(fh, lbs->el_index[index].offset, SEEK_SET);
   i = my_read(fh, message, TEXT_SIZE + 1000 - 1);
   if (i <= 0) {
      xfree(message);
      close(fh);
      return EL_FILE_ERROR;
   }

   message[i] = 0;
   close(fh);

   if (strncmp(message, "$@MID@$:", 8) != 0) {
      xfree(message);
      /* file might have been edited, rebuild index */
      el_build_index(lbs, TRUE);
      return el_retrieve(lbs, message_id, date, attr_list, attrib, n_attr, text, textsize, in_reply_to,
                         reply_to, attachment, encoding, locked_by, draft);
   }

   /* check for correct ID */
   if (atoi(message + 8) != message_id) {
      xfree(message);
      return EL_FILE_ERROR;
   }

   /* decode message size */
   p = strstr(message + 8, "$@MID@$:");
   if (p == NULL)
      size = strlen(message);
   else
      size = p - message;

   message[size] = 0;

   /* decode message */
   if (date)
      el_decode(message, "Date: ", date, 80);
   if (reply_to)
      el_decode_intlist(message, "Reply to: ", reply_to, MAX_REPLY_TO * 10);
   if (in_reply_to)
      el_decode_int(message, "In reply to: ", in_reply_to, 80);

   if (n_attr == -1) {
      /* derive attribute names from message */
      for (i = 0;; i++) {
         el_enum_attr(message, i, attr_list[i], attrib[i]);
         if (!attr_list[i][0])
            break;
      }
      n_attr = i;

   } else {
      if (attrib)
         for (i = 0; i < n_attr; i++) {
            sprintf(str, "%s: ", attr_list[i]);
            el_decode(message, str, attrib[i], NAME_LENGTH);
         }
   }

   el_decode(message, "Attachment: ", attachment_all, sizeof(attachment_all));
   if (encoding)
      el_decode(message, "Encoding: ", encoding, 80);

   if (attachment) {
      /* break apart attachements */
      for (i = 0; i < MAX_ATTACHMENTS; i++)
         if (attachment[i] != NULL)
            attachment[i][0] = 0;

      for (i = 0; i < MAX_ATTACHMENTS; i++) {
         if (attachment[i] != NULL) {
            if (i == 0)
               p = strtok(attachment_all, ",");
            else
               p = strtok(NULL, ",");

            if (p != NULL)
               strcpy(attachment[i], p);
            else
               break;
         }
      }
   }

   if (locked_by)
      el_decode(message, "Locked by: ", locked_by, 80);

   if (draft)
      el_decode(message, "Draft: ", draft, 80);

   p = strstr(message, "========================================\n");

   /* check for \n -> \r conversion (e.g. zipping/unzipping) */
   if (p == NULL)
      p = strstr(message, "========================================\r");

   if (text) {
      if (p != NULL) {
         p += 41;
         if ((int) strlen(p) >= *textsize) {
            strlcpy(text, p, *textsize);
            show_error("Entry too long to display. Please increase TEXT_SIZE and recompile elogd.");
            xfree(message);
            return EL_FILE_ERROR;
         } else {
            strlcpy(text, p, *textsize);

            /* strip CR at end */
            if (text[strlen(text) - 1] == '\n') {
               text[strlen(text) - 1] = 0;
               if (text[strlen(text) - 1] == '\r')
                  text[strlen(text) - 1] = 0;
            }

            *textsize = strlen(text);
         }
      } else {
         text[0] = 0;
         *textsize = 0;
      }
   }

   xfree(message);
   return EL_SUCCESS;
}