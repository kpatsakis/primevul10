int el_retrieve_attachment(LOGBOOK *lbs, int message_id, int n, char name[MAX_PATH_LENGTH]) {
   int i, index, size, fh;
   char file_name[MAX_PATH_LENGTH * 3], *p;
   char message[TEXT_SIZE + 1000], attachment_all[64 * MAX_ATTACHMENTS];

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
      return el_retrieve_attachment(lbs, message_id, n, name);
   }

   lseek(fh, lbs->el_index[index].offset, SEEK_SET);
   i = my_read(fh, message, sizeof(message) - 1);
   if (i <= 0) {
      close(fh);
      return EL_FILE_ERROR;
   }

   message[i] = 0;
   close(fh);

   if (strncmp(message, "$@MID@$:", 8) != 0) {
      /* file might have been edited, rebuild index */
      el_build_index(lbs, TRUE);
      return el_retrieve_attachment(lbs, message_id, n, name);
   }

   /* check for correct ID */
   if (atoi(message + 8) != message_id)
      return EL_FILE_ERROR;

   /* decode message size */
   p = strstr(message + 8, "$@MID@$:");
   if (p == NULL)
      size = strlen(message);
   else
      size = p - message;

   message[size] = 0;

   el_decode(message, "Attachment: ", attachment_all, sizeof(attachment_all));

   name[0] = 0;

   for (i = 0; i <= n; i++) {
      if (i == 0)
         p = strtok(attachment_all, ",");
      else
         p = strtok(NULL, ",");

      if (p == NULL)
         break;
   }

   if (p)
      strlcpy(name, p, MAX_PATH_LENGTH);

   return EL_SUCCESS;
}