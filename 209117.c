void format_email_text(LOGBOOK *lbs, char attrib[MAX_N_ATTR][NAME_LENGTH],
                       char att_file[MAX_ATTACHMENTS][256], int old_mail, char *url, char *multipart_boundary,
                       char *mail_text, int size) {
   int i, j, k, flags, n_email_attr, attr_index[MAX_N_ATTR];
   char str[NAME_LENGTH + 100], str2[256], mail_from[256], mail_from_name[256], format[256],
           list[MAX_N_ATTR][NAME_LENGTH], comment[256], charset[256], heading[256],
           slist[MAX_N_ATTR + 10][NAME_LENGTH], svalue[MAX_N_ATTR + 10][NAME_LENGTH];
   time_t ltime;
   struct tm *pts;

   if (multipart_boundary[0]) {
      if (!getcfg("global", "charset", charset, sizeof(charset)))
         strcpy(charset, DEFAULT_HTTP_CHARSET);

      strlcat(mail_text, "--", size);
      strlcat(mail_text, multipart_boundary, size);
      strlcat(mail_text, "\r\n", size);
      sprintf(mail_text + strlen(mail_text), "Content-Type: text/plain; charset=%s; format=flowed\r\n",
              charset);
      sprintf(mail_text + strlen(mail_text), "Content-Transfer-Encoding: 7bit\r\n\r\n");
   } else
      strlcat(mail_text, "\r\n", size);

   flags = 63;
   if (getcfg(lbs->name, "Email format", str, sizeof(str)))
      flags = atoi(str);

   retrieve_email_from(lbs, mail_from, mail_from_name, attrib);

   if (flags & 1) {
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
            strlcpy(mail_text + strlen(mail_text), loc("An old ELOG entry has been updated"),
                    size - strlen(mail_text));
         else
            strlcpy(mail_text + strlen(mail_text), loc("A new ELOG entry has been submitted"),
                    size - strlen(mail_text));
         strcat(mail_text, ":");
      }

      strlcpy(mail_text + strlen(mail_text), "\r\n\r\n", size - strlen(mail_text));
   }

   if (flags & 32)
      sprintf(mail_text + strlen(mail_text), "%s             : %s\r\n", loc("Logbook"), lbs->name);

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

         if (k < 20)
            sprintf(str + 20, ": %s\r\n", comment);
         else
            sprintf(str + k + 1, ": %s\r\n", comment);

         strcpy(mail_text + strlen(mail_text), str);
      }
   }

   if (flags & 4)
      sprintf(mail_text + strlen(mail_text), "\r\n%s URL         : %s\r\n", loc("Logbook"), url);

   if (flags & 64) {
      for (i = 0; i < MAX_ATTACHMENTS && att_file[i][0]; i++)
         sprintf(mail_text + strlen(mail_text), "\r\n%s %d        : %s (%s/%d)\r\n", loc("Attachment"),
                 i + 1, att_file[i] + 14, url, i + 1);
   }

   if (flags & 8) {
      if (isparam("text")) {
         sprintf(mail_text + strlen(mail_text), "\r\n=================================\r\n\r\n%s",
                 getparam("text"));
      }
   }

   strlcat(mail_text, "\r\n\r\n", size);
}