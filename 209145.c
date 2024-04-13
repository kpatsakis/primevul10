int compose_email(LOGBOOK *lbs, char *rcpt_to, char *mail_to, int message_id,
                  char attrib[MAX_N_ATTR][NAME_LENGTH], char *mail_param, int old_mail,
                  char att_file[MAX_ATTACHMENTS][256], char *encoding, int reply_id) {
   int i, n, flags, status, mail_encoding, mail_text_size, max_att_size, n_attachments, max_allowed_att_size;
   char str[NAME_LENGTH + 100], mail_from[256], mail_from_name[256], *mail_text, smtp_host[256],
           subject[256], error[256];
   char list[MAX_PARAM][NAME_LENGTH], url[256];
   char slist[MAX_N_ATTR + 10][NAME_LENGTH], svalue[MAX_N_ATTR + 10][NAME_LENGTH];
   char multipart_boundary[80];

   if (!getcfg("global", "SMTP host", smtp_host, sizeof(smtp_host))) {
      show_error(loc("No SMTP host defined in [global] section of configuration file"));
      return 0;
   }

   evaluate_conditions(lbs, attrib);

   flags = 63;
   if (getcfg(lbs->name, "Email format", str, sizeof(str)))
      flags = atoi(str);

   /* get initial HTML flag from message encoding */
   mail_encoding = 1;           // 1:text, 2:short HTML, 4:full HTML
   if (encoding[0] == 'E' || encoding[0] == 'H')
      mail_encoding = 4;

   /* overwrite with config setting */
   if (getcfg(lbs->name, "Email encoding", str, sizeof(str)))
      mail_encoding = atoi(str);

   max_allowed_att_size = (int) 10E6;
   if (getcfg(lbs->name, "Max email attachment size", str, sizeof(str)))
      max_allowed_att_size = atoi(str);

   retrieve_email_from(lbs, mail_from, mail_from_name, attrib);

   /* compose subject from attributes */
   if (old_mail && getcfg(lbs->name, "Use Email Subject Edit", subject, sizeof(subject))) {
      i = build_subst_list(lbs, slist, svalue, attrib, TRUE);
      sprintf(str, "%d", message_id);
      add_subst_list(slist, svalue, "message id", str, &i);
      strsubst_list(subject, sizeof(subject), slist, svalue, i);
   } else if (getcfg(lbs->name, "Use Email Subject", subject, sizeof(subject))) {
      i = build_subst_list(lbs, slist, svalue, attrib, TRUE);
      sprintf(str, "%d", message_id);
      add_subst_list(slist, svalue, "message id", str, &i);
      strsubst_list(subject, sizeof(subject), slist, svalue, i);
   } else {
      if (old_mail)
         strcpy(subject, "Updated ELOG entry");
      else
         strcpy(subject, "New ELOG entry");
   }

   /* count attachments and get maximum size */
   n_attachments = 0;
   max_att_size = 0;
   if (att_file) {
      for (i = 0; att_file[i][0] && i < MAX_ATTACHMENTS; i++) {
         if ((mail_encoding & 6) == 0 || !is_inline_attachment(encoding, message_id,
                                                               getparam("text"), i, att_file[i]))
            n_attachments++;
      }
      max_att_size = max_attachment_size(lbs, message_id);
   }

   // if attachments too large, include only links to attachments
   if (max_att_size > max_allowed_att_size && (flags & 16) > 0) {
      flags &= ~(16);
      flags |= 64;
   }

   compose_base_url(lbs, str, sizeof(str), TRUE);
   sprintf(url, "%s%d", str, message_id);

   mail_text_size = MAX_CONTENT_LENGTH + 1000;
   mail_text = xmalloc(mail_text_size);
   mail_text[0] = 0;

   compose_email_header(lbs, subject, mail_from_name, mail_to, url, mail_text, mail_text_size, mail_encoding,
                        n_attachments, multipart_boundary, message_id, reply_id);

   if (mail_encoding & 1)
      format_email_text(lbs, attrib, att_file, old_mail, url, multipart_boundary, mail_text, mail_text_size);
   if (mail_encoding & 2)
      format_email_html(lbs, message_id, attrib, att_file, old_mail, encoding, url, multipart_boundary,
                        mail_text, mail_text_size);
   if (mail_encoding & 4)
      format_email_html2(lbs, message_id, att_file, old_mail, multipart_boundary, mail_text, mail_text_size,
                         flags);

   if (n_attachments && (flags & 16)) {
      if ((mail_encoding & 6) > 0)
         /* only non-inline attachements */
         format_email_attachments(lbs, message_id, 1, att_file, mail_text, mail_text_size,
                                  multipart_boundary, FALSE);
      else
         /* all attachments */
         format_email_attachments(lbs, message_id, 0, att_file, mail_text, mail_text_size,
                                  multipart_boundary, FALSE);
   }

   if (multipart_boundary[0]) {
      strlcat(mail_text, "--", mail_text_size);
      strlcat(mail_text, multipart_boundary, mail_text_size);
      strlcat(mail_text, "--\r\n\r\n", mail_text_size);
   }

   status = sendmail(lbs, smtp_host, mail_from, rcpt_to, mail_text, error, sizeof(error));

   /*
      {
      int fh;
      fh = open("mail.html", O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, 0644);
      write(fh, mail_text, strlen(mail_text));
      close(fh);
      }
    */

   if (status < 0) {
      sprintf(str, loc("Error sending Email via \"%s\""), smtp_host);
      if (error[0]) {
         strlcat(str, ": ", sizeof(str));
         strlcat(str, error, sizeof(str));
      }
      url_encode(str, sizeof(str));
      sprintf(mail_param, "?error=%s", str);
   } else if (error[0]) {
      sprintf(str, loc("Error sending Email via \"%s\""), smtp_host);
      strlcat(str, ": ", sizeof(str));
      strlcat(str, error, sizeof(str));
      url_encode(str, sizeof(str));
      sprintf(mail_param, "?error=%s", str);
   } else {
      if (!getcfg(lbs->name, "Display email recipients", str, sizeof(str)) || atoi(str) == 1) {
         if (mail_param[0] == 0)
            strcpy(mail_param, "?");
         else
            strcat(mail_param, "&");

         /* convert '"',CR,LF,TAB to ' ' */
         while (strchr(mail_to, '"'))
            *strchr(mail_to, '"') = ' ';
         while (strchr(mail_to, '\r'))
            *strchr(mail_to, '\r') = ' ';
         while (strchr(mail_to, '\n'))
            *strchr(mail_to, '\n') = ' ';
         while (strchr(mail_to, '\t'))
            *strchr(mail_to, '\t') = ' ';

         n = strbreak(mail_to, list, MAX_PARAM, ",", FALSE);

         if (n < 10) {
            for (i = 0; i < n && i < MAX_PARAM; i++) {
               strlcpy(str, list[i], sizeof(str));
               url_encode(str, sizeof(str));
               sprintf(mail_param + strlen(mail_param), "mail%d=%s", i, str);
               if (i < n - 1)
                  strcat(mail_param, "&");
            }
         } else {
            sprintf(str, "%d%%20%s", n, loc("recipients"));
            sprintf(mail_param + strlen(mail_param), "mail0=%s", str);
         }
      }
   }

   xfree(mail_text);

   return status;
}