void compose_email_header(LOGBOOK *lbs, char *subject, char *from, char *to, char *url, char *mail_text,
                          int size, int mail_encoding, int n_attachments, char *multipart_boundary,
                          int message_id, int reply_id) {
   char buffer[256], charset[256], subject_enc[5000];
   char buf[80], str[256];
   int i, offset, multipart;
   time_t now;
   struct tm *ts;

   i = 0;
   if (mail_encoding & 1)
      i++;
   if (mail_encoding & 2)
      i++;
   if (mail_encoding & 4)
      i++;
   multipart = i > 1;

   if (!getcfg("global", "charset", charset, sizeof(charset)))
      strcpy(charset, DEFAULT_HTTP_CHARSET);

   /* switch locale temporarily back to english to comply with RFC2822 date format */
   setlocale(LC_ALL, "C");

   time(&now);
   ts = localtime(&now);
   assert(ts);
   strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", ts);
   offset = (-(int) my_timezone());
   if (ts->tm_isdst)
      offset += 3600;
   if (get_verbose() >= VERBOSE_INFO) {
      sprintf(str, "timezone: %d, offset: %d\n", (int) my_timezone(), (int) offset);
      efputs(str);
   }
   snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1, "Date: %s %+03d%02d\r\n", buf,
            (int) (offset / 3600), (int) ((abs((int) offset) / 60) % 60));

   getcfg("global", "Language", str, sizeof(str));
   if (str[0])
      setlocale(LC_ALL, str);

   strlcat(mail_text, "To: ", size);

   if (getcfg(lbs->name, "Omit Email to", str, sizeof(str)) && atoi(str) == 1)
      strlcat(mail_text, "ELOG", size);
   else
      strlcat(mail_text, to, size);

   strlcat(mail_text, "\r\n", size);

   snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1, "From: %s\r\n", from);
   snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1, "User-Agent: Elog Version %s\r\n",
            VERSION);

   strlcat(mail_text, "MIME-Version: 1.0\r\n", size);

   memset(subject_enc, 0, sizeof(subject_enc));

   for (i = 0; i < (int) strlen(subject); i++)
      if (subject[i] < 0)
         break;

   if (i < (int) strlen(subject)) {
      /* subject contains local characters, so encode it using charset */
      for (i = 0; i < (int) strlen(subject); i += 40) {
         strlcpy(buffer, subject + i, sizeof(buffer));
         buffer[40] = 0;
         strlcat(subject_enc, "=?", sizeof(subject_enc));
         strlcat(subject_enc, charset, sizeof(subject_enc));
         strlcat(subject_enc, "?B?", sizeof(subject_enc));
         base64_encode((unsigned char *) buffer, (unsigned char *) (subject_enc + strlen(subject_enc)),
                       sizeof(subject_enc) - strlen(subject_enc));
         strlcat(subject_enc, "?=", sizeof(subject_enc));
         if (strlen(subject + i) < 40)
            break;

         strlcat(subject_enc, "\r\n ", sizeof(subject_enc));    // another encoded-word
      }
   } else
      strlcpy(subject_enc, subject, sizeof(subject_enc));

   snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1, "Subject: %s\r\n", subject_enc);

   if (strchr(from, '@')) {
      strlcpy(str, strchr(from, '@') + 1, sizeof(str));
      if (strchr(str, '>'))
         *strchr(str, '>') = 0;
   } else
      strlcpy(str, "elog.org", sizeof(str));

   if (message_id)
      snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1, "Message-ID: <%s-%d@%s>\r\n",
               lbs->name_enc, message_id, str);
   if (reply_id)
      snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1, "In-Reply-To: <%s-%d@%s>\r\n",
               lbs->name_enc, reply_id, str);

   if (url)
      snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1, "X-Elog-URL: %s\r\n", url);

   strlcat(mail_text, "X-Elog-submit-type: web|elog\r\n", size);

   if (multipart) {

      sprintf(multipart_boundary, "------------%04X%04X%04X", rand(), rand(), rand());
      snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1,
               "MIME-Version: 1.0\r\nContent-Type: multipart/alternative;\r\n boundary=\"%s\"\r\n\r\n",
               multipart_boundary);

      strlcat(mail_text, "This is a multi-part message in MIME format.\r\n", size);
   } else {
      if (n_attachments) {
         sprintf(multipart_boundary, "------------%04X%04X%04X", rand(), rand(), rand());
         snprintf(mail_text + strlen(mail_text), size - strlen(mail_text) - 1,
                  "MIME-Version: 1.0\r\nContent-Type: multipart/mixed;\r\n boundary=\"%s\"\r\n\r\n",
                  multipart_boundary);

         strlcat(mail_text, "This is a multi-part message in MIME format.\r\n", size);
      } else {
         if (multipart_boundary)
            multipart_boundary[0] = 0;
      }
   }
}