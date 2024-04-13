void format_email_html2(LOGBOOK *lbs, int message_id, char att_file[MAX_ATTACHMENTS][256], int old_mail,
                        char *multipart_boundary, char *mail_text, int size, int flags) {
   char str[256], charset[256], multipart_boundary_related[256], command[256], *p;
   int max_att_size;

   sprintf(str, "%d", message_id);
   if (!getcfg("global", "charset", charset, sizeof(charset)))
      strcpy(charset, DEFAULT_HTTP_CHARSET);

   if (multipart_boundary[0]) {
      strlcat(mail_text, "--", size);
      strlcat(mail_text, multipart_boundary, size);
      strlcat(mail_text, "\r\n", size);
   }

   max_att_size = max_attachment_size(lbs, message_id);

   if (max_att_size && (flags & 16) > 0) {
      sprintf(multipart_boundary_related, "------------%04X%04X%04X", rand(), rand(), rand());
      snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1,
               "MIME-Version: 1.0\r\nContent-Type: multipart/related;\r\n boundary=\"%s\"\r\n\r\n",
               multipart_boundary_related);
      strlcat(mail_text, "--", size);
      strlcat(mail_text, multipart_boundary_related, size);
      strlcat(mail_text, "\r\n", size);
   }

   snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1,
            "Content-Type: text/html; charset=\"%s\"\r\n", charset);
   snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1,
            "Content-Transfer-Encoding: 7bit\r\n\r\n");

   strlen_retbuf = 0;
   if (old_mail)
      strcpy(command, "oldemail");
   else
      strcpy(command, "email");

   if ((flags & 64) > 0)
      strlcat(command, "-att-links", sizeof(command));

   show_elog_entry(lbs, str, command);

   p = strstr(return_buffer, "\r\n\r\n");
   if (p)
      strlcpy(mail_text + strlen(mail_text), p + 4, size - strlen(mail_text));
   strlen_retbuf = 0;
   strlcat(mail_text, "\r\n", size);

   if (max_att_size && (flags & 16) > 0) {
      format_email_attachments(lbs, message_id, 2, att_file, mail_text, size, multipart_boundary_related,
                               TRUE);
      strlcat(mail_text, "--", size);
      strlcat(mail_text, multipart_boundary_related, size);
      strlcat(mail_text, "--\r\n\r\n", size);
   }
}