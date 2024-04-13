int sendmail(LOGBOOK *lbs, char *smtp_host, char *from, char *to, char *text, char *error, int error_size) {
   struct sockaddr_in bind_addr;
   struct hostent *phe;
   int i, n, s, strsize;
   char *str;
   char list[MAX_N_EMAIL][NAME_LENGTH], buffer[10000], decoded[256];

   memset(error, 0, error_size);

   if (get_verbose() >= VERBOSE_INFO)
      eprintf("\n\nEmail from %s to %s, SMTP host %s:\n", from, to, smtp_host);
   sprintf(buffer, "Email from %s to ", from);
   strlcat(buffer, to, sizeof(buffer));
   strlcat(buffer, ", SMTP host ", sizeof(buffer));
   strlcat(buffer, smtp_host, sizeof(buffer));
   strlcat(buffer, ":\n", sizeof(buffer));
   write_logfile(lbs, buffer);

   /* create a new socket for connecting to remote server */
   s = socket(AF_INET, SOCK_STREAM, 0);
   if (s == -1)
      return -1;

   strsize = MAX_CONTENT_LENGTH + 1000;
   str = xmalloc(strsize);

   /* connect to remote node port on SMTP port */
   int smtp_port = 25;
   if (getcfg(lbs->name, "SMTP port", str, strsize))
      smtp_port = atoi(str);

   memset(&bind_addr, 0, sizeof(bind_addr));
   bind_addr.sin_family = AF_INET;
   bind_addr.sin_port = htons((short) smtp_port);

   phe = gethostbyname(smtp_host);
   if (phe == NULL) {
      if (error)
         strlcpy(error, loc("Cannot lookup server name"), error_size);
      return -1;
   }
   memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);

   if (connect(s, (void *) &bind_addr, sizeof(bind_addr)) < 0) {
      closesocket(s);
      if (error)
         strlcpy(error, loc("Cannot connect to server"), error_size);
      return -1;
   }

   recv_string(s, str, strsize, 10000);
   if (get_verbose() >= VERBOSE_INFO)
      efputs(str);
   write_logfile(lbs, str);

   /* drain server messages */
   do {
      str[0] = 0;
      recv_string(s, str, strsize, 300);
      if (get_verbose() >= VERBOSE_INFO)
         efputs(str);
      write_logfile(lbs, str);
   } while (str[0]);

   if (getcfg(lbs->name, "SMTP username", str, strsize)) {

      snprintf(str, strsize - 1, "EHLO %s\r\n", host_name);
      send(s, str, strlen(str), 0);
      if (get_verbose() >= VERBOSE_INFO)
         efputs(str);
      write_logfile(lbs, str);

      do {
         recv_string(s, str, strsize, 3000);
         if (get_verbose() >= VERBOSE_INFO)
            efputs(str);
         write_logfile(lbs, str);
         if (!check_smtp_error(str, 250, error, error_size))
            goto smtp_error;
      } while (stristr(str, "250 ") == NULL);

   } else {

      snprintf(str, strsize - 1, "HELO %s\r\n", host_name);

      send(s, str, strlen(str), 0);
      if (get_verbose() >= VERBOSE_INFO)
         efputs(str);
      write_logfile(lbs, str);
      recv_string(s, str, strsize, 3000);
      if (get_verbose() >= VERBOSE_INFO)
         efputs(str);
      write_logfile(lbs, str);
      if (!check_smtp_error(str, 250, error, error_size))
         goto smtp_error;
   }

   /* optional authentication */
   if (getcfg(lbs->name, "SMTP username", str, strsize)) {

      snprintf(str, strsize - 1, "AUTH LOGIN\r\n");
      send(s, str, strlen(str), 0);
      if (get_verbose() >= VERBOSE_INFO)
         efputs(str);
      write_logfile(lbs, str);
      recv_string(s, str, strsize, 3000);
      if (strchr(str, '\r'))
         *strchr(str, '\r') = 0;
      if (atoi(str) != 334) {
         strcat(str, "\n");
         if (get_verbose() >= VERBOSE_INFO)
            efputs(str);
         write_logfile(lbs, str);
      } else {
         base64_decode(str + 4, decoded);
         strcat(decoded, "\n");
         if (get_verbose() >= VERBOSE_INFO)
            efputs(decoded);
         write_logfile(lbs, decoded);
      }
      if (!check_smtp_error(str, 334, error, error_size))
         goto smtp_error;

      getcfg(lbs->name, "SMTP username", decoded, sizeof(decoded));
      base64_encode((unsigned char *) decoded, (unsigned char *) str, strsize);
      strcat(str, "\r\n");
      send(s, str, strlen(str), 0);
      if (get_verbose() >= VERBOSE_INFO)
         efputs(decoded);
      write_logfile(lbs, decoded);
      recv_string(s, str, strsize, 3000);
      if (strchr(str, '\r'))
         *strchr(str, '\r') = 0;
      base64_decode(str + 4, decoded);
      strcat(decoded, "\n");
      if (get_verbose() >= VERBOSE_INFO)
         efputs(decoded);
      write_logfile(lbs, decoded);
      if (!check_smtp_error(str, 334, error, error_size))
         goto smtp_error;

      getcfg(lbs->name, "SMTP password", str, strsize);
      strcat(str, "\r\n");
      send(s, str, strlen(str), 0);
      if (get_verbose() >= VERBOSE_INFO)
         efputs(str);
      write_logfile(lbs, str);
      recv_string(s, str, strsize, 3000);
      if (get_verbose() >= VERBOSE_INFO)
         efputs(str);
      write_logfile(lbs, str);
      if (!check_smtp_error(str, 235, error, error_size))
         goto smtp_error;
   }

   snprintf(str, strsize - 1, "MAIL FROM: %s\r\n", from);
   send(s, str, strlen(str), 0);
   if (get_verbose() >= VERBOSE_INFO)
      efputs(str);
   write_logfile(lbs, str);
   recv_string(s, str, strsize, 3000);
   if (get_verbose() >= VERBOSE_INFO)
      efputs(str);
   write_logfile(lbs, str);

   if (!check_smtp_error(str, 250, error, error_size))
      goto smtp_error;

   /* break recipients into list */
   n = strbreak(to, list, MAX_N_EMAIL, ",", FALSE);

   for (i = 0; i < n; i++) {
      if (list[i] == 0 || strchr(list[i], '@') == NULL)
         continue;

      snprintf(str, strsize - 1, "RCPT TO: <%s>\r\n", list[i]);
      send(s, str, strlen(str), 0);
      if (get_verbose() >= VERBOSE_INFO)
         efputs(str);
      write_logfile(lbs, str);

      /* increased timeout for SMTP servers with long alias lists */
      recv_string(s, str, strsize, 30000);
      if (get_verbose() >= VERBOSE_INFO)
         efputs(str);
      write_logfile(lbs, str);

      if (!check_smtp_error(str, 250, error, error_size))
         goto smtp_error;
   }

   snprintf(str, strsize - 1, "DATA\r\n");
   send(s, str, strlen(str), 0);
   if (get_verbose() >= VERBOSE_INFO)
      efputs(str);
   write_logfile(lbs, str);
   recv_string(s, str, strsize, 3000);
   if (get_verbose() >= VERBOSE_INFO)
      efputs(str);
   write_logfile(lbs, str);
   if (!check_smtp_error(str, 354, error, error_size))
      goto smtp_error;

   /* replace "." at beginning of line by ".." */
   strlcpy(str, text, strsize);
   strsubst(str, strsize, "\n.", "\n..");

   /* add ".<CR>" to signal end of message */
   strlcat(str, ".\r\n", strsize);

   /* check if buffer exceeded */
   if ((int) strlen(str) == strsize - 1) {
      strlcpy(error, loc("Entry size too large for email notification"), error_size);
      goto smtp_error;
   }

   send(s, str, strlen(str), 0);
   if (get_verbose() >= VERBOSE_INFO)
      efputs(str);
   write_logfile(lbs, str);
   recv_string(s, str, strsize, 10000);
   if (get_verbose() >= VERBOSE_INFO)
      efputs(str);
   write_logfile(lbs, str);
   if (!check_smtp_error(str, 250, error, error_size))
      goto smtp_error;

   snprintf(str, strsize - 1, "QUIT\r\n");
   send(s, str, strlen(str), 0);
   if (get_verbose() >= VERBOSE_INFO)
      efputs(str);
   write_logfile(lbs, str);
   recv_string(s, str, strsize, 3000);
   if (get_verbose() >= VERBOSE_INFO)
      efputs(str);
   write_logfile(lbs, str);
   if (!check_smtp_error(str, 221, error, error_size))
      goto smtp_error;

   closesocket(s);
   xfree(str);
   return 1;

   smtp_error:

   closesocket(s);
   xfree(str);

   return -1;
}