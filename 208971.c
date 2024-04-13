int el_delete_message(LOGBOOK *lbs, int message_id, BOOL delete_attachments,
                      char attachment[MAX_ATTACHMENTS][MAX_PATH_LENGTH], BOOL delete_bw_ref,
                      BOOL delete_reply_to)
/********************************************************************
 Routine: el_delete_message

 Purpose: Delete an ELog entry including attachments

 Input:
 LOGBOOK *lbs          Pointer to logbook structure
 int     message_id    Message ID
 BOOL    delete_attachments   Delete attachments if TRUE
 char    attachment    Used to return attachments (on move)
 BOOL    delete_bw_ref If true, delete backward references
 BOOL    delete_reply_to If true, delete replies to this message

 Output:
 <none>

 Function value:
 EL_SUCCESS              Successful completion

 \********************************************************************/
{
   int i, index, size, fh, tail_size, old_offset;
   char str[MAX_PATH_LENGTH], file_name[MAX_PATH_LENGTH * 3], reply_to[MAX_REPLY_TO * 10], in_reply_to[256];
   char *buffer, *p;
   char *message, attachment_all[64 * MAX_ATTACHMENTS];
   char attrib[MAX_N_ATTR][NAME_LENGTH];

   for (index = 0; index < *lbs->n_el_index; index++)
      if (lbs->el_index[index].message_id == message_id)
         break;

   if (index == *lbs->n_el_index)
      return -1;

   snprintf(file_name, sizeof(file_name), "%s%s%s", lbs->data_dir, lbs->el_index[index].subdir,
            lbs->el_index[index].file_name);
   fh = open(file_name, O_RDWR | O_BINARY, 0644);
   if (fh < 0)
      return EL_FILE_ERROR;

   message = xmalloc(TEXT_SIZE + 1000);

   lseek(fh, lbs->el_index[index].offset, SEEK_SET);
   i = my_read(fh, message, TEXT_SIZE + 1000 - 1);
   if (i <= 0) {
      xfree(message);
      close(fh);
      return EL_FILE_ERROR;
   }

   if (_logging_level > 1) {
      sprintf(str, "DELETE entry #%d", message_id);
      write_logfile(lbs, str);
   }

   message[i] = 0;

   if (strncmp(message, "$@MID@$:", 8) != 0) {
      close(fh);
      xfree(message);

      /* file might have been edited, rebuild index */
      el_build_index(lbs, TRUE);
      return el_delete_message(lbs, message_id, delete_attachments, attachment, delete_bw_ref,
                               delete_reply_to);
   }

   /* check for correct ID */
   if (atoi(message + 8) != message_id) {
      close(fh);
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

   /* delete attachments */
   el_decode(message, "Attachment: ", attachment_all, sizeof(attachment_all));

   for (i = 0; i < MAX_ATTACHMENTS; i++) {
      if (i == 0)
         p = strtok(attachment_all, ",");
      else
         p = strtok(NULL, ",");

      if (attachment != NULL) {
         if (attachment[i][0] && p) {
            /* delete old attachment if new one exists */
            el_delete_attachment(lbs, p);
         }

         /* return old attachment if no new one */
         if (!attachment[i][0] && p)
            strcpy(attachment[i], p);
      }

      if (delete_attachments && p)
         el_delete_attachment(lbs, p);
   }

   /* decode references */
   el_decode_intlist(message, "Reply to: ", reply_to, sizeof(reply_to));
   el_decode_int(message, "In reply to: ", in_reply_to, sizeof(in_reply_to));

   /* decoded attributes */
   for (i = 0;; i++) {
      el_enum_attr(message, i, attr_list[i], attrib[i]);
      if (!attr_list[i][0])
         break;
   }

   /* buffer tail of logfile */
   lseek(fh, 0, SEEK_END);
   tail_size = TELL(fh) - (lbs->el_index[index].offset + size);

   buffer = NULL;
   if (tail_size > 0) {
      buffer = xmalloc(tail_size);

      lseek(fh, lbs->el_index[index].offset + size, SEEK_SET);
      my_read(fh, buffer, tail_size);
   }
   lseek(fh, lbs->el_index[index].offset, SEEK_SET);

   if (tail_size > 0) {
      write(fh, buffer, tail_size);
      xfree(buffer);
   }

   /* truncate file here */
   TRUNCATE(fh);

   /* if file length gets zero, delete file */
   tail_size = lseek(fh, 0, SEEK_END);
   close(fh);
   xfree(message);

   if (tail_size == 0)
      remove(file_name);

   /* remove message from index */
   strcpy(str, lbs->el_index[index].file_name);
   old_offset = lbs->el_index[index].offset;
   for (i = index; i < *lbs->n_el_index - 1; i++)
      memcpy(&lbs->el_index[i], &lbs->el_index[i + 1], sizeof(EL_INDEX));

   (*lbs->n_el_index)--;
   if (*lbs->n_el_index > 0)
      lbs->el_index = xrealloc(lbs->el_index, sizeof(EL_INDEX) * (*lbs->n_el_index));

   /* correct all offsets after deleted message */
   for (i = 0; i < *lbs->n_el_index; i++)
      if (strieq(lbs->el_index[i].file_name, str) && lbs->el_index[i].offset > old_offset)
         lbs->el_index[i].offset -= size;

   /* if other logbook has same index, update pointers */
   for (i = 0; lb_list[i].name[0]; i++)
      if (&lb_list[i] != lbs && lb_list[i].n_el_index == lbs->n_el_index)
         lb_list[i].el_index = lbs->el_index;

   /* delete also replies to this message */
   if (delete_reply_to && reply_to[0]) {
      p = reply_to;
      if (isdigit(*p))
         do {
            if (atoi(p))
               el_delete_message(lbs, atoi(p), TRUE, NULL, FALSE, TRUE);

            while (*p && isdigit(*p))
               p++;
            while (*p && (*p == ',' || *p == ' '))
               p++;
         } while (*p);
   }

   /* delete backward references */
   if (in_reply_to[0] && delete_bw_ref) {
      p = in_reply_to;
      do {
         if (atoi(p))
            remove_reference(lbs, atoi(p), message_id, TRUE);

         while (*p && isdigit(*p))
            p++;
         while (*p && (*p == ',' || *p == ' '))
            p++;
      } while (*p);
   }

   /* execute shell if requested */
   if (getcfg(lbs->name, "Execute delete", str, sizeof(str)))
      execute_shell(lbs, message_id, attrib, NULL, str);

   return EL_SUCCESS;
}