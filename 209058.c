void format_email_html(LOGBOOK *lbs, int message_id, char attrib[MAX_N_ATTR][NAME_LENGTH],
                       char att_file[MAX_ATTACHMENTS][256], int old_mail, char *encoding, char *url,
                       char *multipart_boundary, char *mail_text, int size) {
   int i, j, k, flags, n_email_attr, attr_index[MAX_N_ATTR], max_att_size, max_allowed_att_size;
   char str[NAME_LENGTH + 100], str2[256], mail_from[256], mail_from_name[256], format[256],
           list[MAX_N_ATTR][NAME_LENGTH], comment[256], charset[256], multipart_boundary_related[256],
           heading[256], slist[MAX_N_ATTR + 10][NAME_LENGTH], svalue[MAX_N_ATTR + 10][NAME_LENGTH];
   time_t ltime;
   struct tm *pts;

   if (!getcfg("global", "charset", charset, sizeof(charset)))
      strcpy(charset, DEFAULT_HTTP_CHARSET);

   if (multipart_boundary[0]) {
      strlcat(mail_text, "--", size);
      strlcat(mail_text, multipart_boundary, size);
      strlcat(mail_text, "\r\n", size);
   }

   max_att_size = max_attachment_size(lbs, message_id);
   max_allowed_att_size = (int) 10E6;
   if (getcfg(lbs->name, "Max email attachment size", str, sizeof(str)))
      max_allowed_att_size = atoi(str);

   if (max_att_size) {
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

   retrieve_email_from(lbs, mail_from, mail_from_name, attrib);

   flags = 63;
   if (getcfg(lbs->name, "Email format", str, sizeof(str)))
      flags = atoi(str);

   // if attachments too large, include only links to attachments
   if (max_att_size > max_allowed_att_size && (flags & 16) > 0) {
      flags &= ~(16);
      flags |= 64;
   }

   strcpy(mail_text + strlen(mail_text),
          "<!DOCTYPE html>\r\n");
   strcpy(mail_text + strlen(mail_text), "<html>\r\n<head>\r\n  <title></title>\r\n</head>\r\n<body>\r\n");

   if (flags & 1) {
      strcpy(mail_text + strlen(mail_text), "<h3>\r\n");
      if (getcfg(lbs->name, "Use Email heading", heading, sizeof(heading))) {
         if (old_mail) {
            if (!getcfg(lbs->name, "Use Email heading edit", heading, sizeof(heading)))
               getcfg(lbs->name, "Use Email heading", heading, sizeof(heading));
         }

         i = build_subst_list(lbs, slist, svalue, attrib, TRUE);
         strsubst_list(heading, sizeof(heading), slist, svalue, i);

         strlcpy(mail_text + strlen(mail_text), heading, size - strlen(mail_text));

      } else {
         if (old_mail)
            sprintf(mail_text + strlen(mail_text), loc("A old entry has been updated on %s"), host_name);
         else
            sprintf(mail_text + strlen(mail_text), loc("A new entry has been submitted on %s"), host_name);
         strcat(mail_text, ":");
      }

      strlcpy(mail_text + strlen(mail_text), "</h3>\r\n", size - strlen(mail_text));
   }

   strlcpy(mail_text + strlen(mail_text), "<table border=\"3\" cellpadding=\"4\">\r\n",
           size - strlen(mail_text));

   if (flags & 32) {
      sprintf(mail_text + strlen(mail_text), "<tr><td bgcolor=\"#CCCCFF\">%s</td>", loc("Logbook"));
      sprintf(mail_text + strlen(mail_text), "<td bgcolor=\"#DDEEBB\">%s</td></tr>\r\n", lbs->name);
   }

   if (flags & 2) {

      if (getcfg(lbs->name, "Email attributes", str, sizeof(str))) {
         n_email_attr = strbreak(str, list, MAX_N_ATTR, ",", FALSE);
         for (i = 0; i < n_email_attr; i++) {
            for (j = 0; j < lbs->n_attr; j++)
               if (strieq(attr_list[j], list[i]))
                  break;
            if (!strieq(attr_list[j], list[i]))
               /* attribute not found */
               j = 0;
            attr_index[i] = j;
         }
      } else {
         for (i = 0; i < lbs->n_attr; i++)
            attr_index[i] = i;
         n_email_attr = lbs->n_attr;
      }

      for (j = 0; j < n_email_attr; j++) {

         i = attr_index[j];
         strcpy(str, "                                                                       ");
         memcpy(str, attr_list[i], strlen(attr_list[i]));

         comment[0] = 0;
         if (attr_flags[i] & AF_ICON) {

            sprintf(str2, "Icon comment %s", attrib[i]);
            getcfg(lbs->name, str2, comment, sizeof(comment));

         } else if (attr_flags[i] & AF_DATE) {

            sprintf(str, "Date format %s", attr_list[i]);
            if (!getcfg(lbs->name, str, format, sizeof(format)))
               if (!getcfg(lbs->name, "Date format", format, sizeof(format)))
                  strcpy(format, DEFAULT_DATE_FORMAT);

            ltime = atoi(attrib[i]);
            pts = localtime(&ltime);
            assert(pts);
            if (ltime == 0)
               strcpy(comment, "-");
            else
               my_strftime(comment, sizeof(str), format, pts);
         } else if (attr_flags[i] & AF_DATETIME) {

            sprintf(str, "Time format %s", attr_list[i]);
            if (!getcfg(lbs->name, str, format, sizeof(format)))
               if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
                  strcpy(format, DEFAULT_TIME_FORMAT);

            ltime = atoi(attrib[i]);
            pts = localtime(&ltime);
            assert(pts);
            if (ltime == 0)
               strcpy(comment, "-");
            else
               my_strftime(comment, sizeof(str), format, pts);
         }

         if (!comment[0])
            strcpy(comment, attrib[i]);

         if (strieq(attr_options[i][0], "boolean"))
            strcpy(comment, atoi(attrib[i]) ? "1" : "0");

         for (k = strlen(str) - 1; k > 0; k--)
            if (str[k] != ' ')
               break;

         sprintf(mail_text + strlen(mail_text), "<tr><td bgcolor=\"#CCCCFF\">%s</td>", attr_list[i]);
         sprintf(mail_text + strlen(mail_text), "<td bgcolor=\"#DDEEBB\">%s</td></tr>\r\n", comment);
      }
   }

   if (flags & 4) {
      sprintf(mail_text + strlen(mail_text),
              "<tr><td bgcolor=\"#CCCCFF\">%s URL</td><td bgcolor=\"#DDEEBB\">", loc("Logbook"));
      sprintf(mail_text + strlen(mail_text), "<a href=\"%s\">%s</a></td></tr>\r\n", url, url);
   }

   if (flags & 64) {
      for (i = 0; i < MAX_ATTACHMENTS && att_file[i][0]; i++) {
         sprintf(mail_text + strlen(mail_text),
                 "<tr><td bgcolor=\"#CCCCFF\">%s %d</td><td bgcolor=\"#DDEEBB\">", loc("Attachment"), i + 1);
         sprintf(mail_text + strlen(mail_text), "<a href=\"%s/%d\">%s</a></td></tr>\r\n", url, i + 1,
                 att_file[i] + 14);
      }
   }

   sprintf(mail_text + strlen(mail_text), "</table>\r\n");

   if (flags & 8) {
      if (isparam("text")) {
         if (encoding[0] == 'H')
            sprintf(mail_text + strlen(mail_text), "\r\n<HR>\r\n%s", getparam("text"));
         else if (encoding[0] == 'E') {
            sprintf(mail_text + strlen(mail_text), "\r\n<HR>\r\n");
            strlen_retbuf = 0;
            rsputs_elcode(lbs, TRUE, getparam("text"));
            strlcpy(mail_text + strlen(mail_text), return_buffer, TEXT_SIZE + 1000 - strlen(mail_text));
            strlen_retbuf = 0;
         } else
            sprintf(mail_text + strlen(mail_text), "\r\n=================================\r\n\r\n%s",
                    getparam("text"));
      }
   }

   strcpy(mail_text + strlen(mail_text), "\r\n</html></body>\r\n\r\n");

   if (max_att_size && (flags & 64)) {
      format_email_attachments(lbs, message_id, 2, att_file, mail_text, size, multipart_boundary_related,
                               TRUE);
      strlcat(mail_text, "--", size);
      strlcat(mail_text, multipart_boundary_related, size);
      strlcat(mail_text, "--\r\n\r\n", size);
   }
}