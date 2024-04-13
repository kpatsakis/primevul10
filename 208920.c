void format_email_attachments(LOGBOOK *lbs, int message_id, int attachment_type,
                              char att_file[MAX_ATTACHMENTS][256], char *mail_text, int size,
                              char *multipart_boundary, int content_id) {
   int i, index, n_att, fh, n, is_inline, length;
   char str[256], file_name[256], buffer[256], domain[256], subdir[256];

   /* count attachments */
   for (n_att = 0; att_file[n_att][0] && n_att < MAX_ATTACHMENTS; n_att++);

   for (index = 0; index < MAX_ATTACHMENTS; index++) {

      if (att_file[index][0] == 0)
         continue;

      is_inline = is_inline_attachment(getparam("encoding"), message_id,
                                       getparam("text"), index, att_file[index]);
      if (attachment_type == 1 && is_inline)
         continue;
      if (attachment_type == 2 && !is_inline)
         continue;

      snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1, "\r\n--%s\r\n",
               multipart_boundary);

      /* return proper Content-Type for file type */
      for (i = 0; i < (int) strlen(att_file[index]) && i < (int) sizeof(str) - 1; i++)
         str[i] = toupper(att_file[index][i]);
      str[i] = 0;

      for (i = 0; filetype[i].ext[0]; i++)
         if (strstr(str, filetype[i].ext))
            break;

      if (filetype[i].ext[0])
         snprintf(str, sizeof(str), "Content-Type: %s; name=\"%s\"\r\n", filetype[i].type, att_file[index]
                                                                                           + 14);
      else if (strchr(str, '.') == NULL)
         snprintf(str, sizeof(str), "Content-Type: text/plain; name=\"%s\"\r\n", att_file[index] + 14);
      else
         snprintf(str, sizeof(str), "Content-Type: application/octet-stream; name=\"%s\"\r\n",
                  att_file[index] + 14);

      strlcat(mail_text, str, size);

      strlcat(mail_text, "Content-Transfer-Encoding: BASE64\r\n", size);

      if (content_id) {
         retrieve_domain(domain, sizeof(domain));
         snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1, "Content-ID: <att%d@%s>\r\n",
                  index, domain);
         snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1,
                  "Content-Disposition: inline; filename=\"%s\"\r\n\r\n", att_file[index] + 14);
      } else
         snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1,
                  "Content-Disposition: attachment; filename=\"%s\"\r\n\r\n", att_file[index] + 14);

      /* encode file */
      strlcpy(file_name, lbs->data_dir, sizeof(file_name));
      generate_subdir_name(att_file[index], subdir, sizeof(subdir));
      strlcat(file_name, subdir, sizeof(file_name));
      strlcat(file_name, att_file[index], sizeof(file_name));
      if (is_image(file_name)) {
         get_thumb_name(file_name, str, sizeof(str), 0);
         if (str[0])
            strlcpy(file_name, str, sizeof(file_name));
      }

      fh = open(file_name, O_RDONLY | O_BINARY);
      length = strlen(mail_text);
      if (fh > 0) {
         do {
            n = my_read(fh, buffer, 45);
            if (n <= 0)
               break;

            base64_bufenc((unsigned char *) buffer, n, str);

            if (length + (int) strlen(str) + 2 < size) {
               strcpy(mail_text + length, str);
               length += strlen(str);
               strcpy(mail_text + length, "\r\n");
               length += 2;
            }
         } while (1);

         close(fh);
      }
   }
}