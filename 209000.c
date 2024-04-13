void receive_config(LOGBOOK *lbs, char *server, char *error_str) {
   char str[256], pwd[256], *buffer, *p;
   int status, version, ssl;

   error_str[0] = 0;

   do {

      combine_url(lbs, server, "", str, sizeof(str), &ssl);
      if (lbs == NULL)
         strcat(str, "?cmd=GetConfig"); // request complete config file
      else
         strcat(str, "?cmd=Download");  // request config section of logbook

      if (retrieve_url(lbs, str, ssl, &buffer, TRUE) < 0) {
         *strchr(str, '?') = 0;
         sprintf(error_str, "Cannot contact elogd server at http://%s", str);
         return;
      }

      /* check version */
      p = strstr(buffer, "ELOG HTTP ");
      if (!p) {
         if (get_verbose() >= VERBOSE_INFO)
            puts(buffer);
         sprintf(error_str, "Remote server is not an ELOG server");
         xfree(buffer);
         return;
      }
      version = atoi(p + 10) * 100 + atoi(p + 12) * 10 + atoi(p + 14);
      if (version < 254) {
         if (get_verbose() >= VERBOSE_INFO)
            puts(buffer);

         strlcpy(str, p + 10, 10);
         if (strchr(str, '\r'))
            *strchr(str, '\r') = 0;

         sprintf(error_str, "Incorrect remote ELOG server version %s, must be 2.5.4 or later", str);
         xfree(buffer);
         return;
      }

      /* evaluate status */
      p = strchr(buffer, ' ');
      if (p == NULL) {
         if (get_verbose() >= VERBOSE_INFO)
            puts(buffer);
         xfree(buffer);
         *strchr(str, '?') = 0;
         sprintf(error_str, "Received invalid response from elogd server at http%s://%s", ssl ? "s" : "",
                 str);
         xfree(buffer);
         return;
      }
      p++;
      status = atoi(p);
      if (status == 401) {
         if (get_verbose() >= VERBOSE_INFO)
            puts(buffer);
         xfree(buffer);
         eprintf("Please enter password to access remote elogd server: ");
         fgets(pwd, sizeof(pwd), stdin);
         while (pwd[strlen(pwd) - 1] == '\n' || pwd[strlen(pwd) - 1] == '\r')
            pwd[strlen(pwd) - 1] = 0;
      } else if (status != 200) {
         if (get_verbose() >= VERBOSE_INFO)
            puts(buffer);
         xfree(buffer);
         *strchr(str, '?') = 0;
         sprintf(error_str, "Received invalid response from elogd server at http%s://%s", ssl ? "s" : "",
                 str);
         return;
      }

   } while (status != 200);

   p = strstr(buffer, "\r\n\r\n");
   if (p == NULL) {
      if (get_verbose() >= VERBOSE_INFO)
         puts(buffer);
      xfree(buffer);
      sprintf(error_str, loc("Cannot receive \"%s\""), str);
      return;
   }
   p += 4;

   if (strstr(p, "[global]") == NULL) {
      strlcpy(error_str, p, 256);
      xfree(buffer);
      return;
   }

   if (lbs == NULL) {
      if (!save_config(p, str))
         rsprintf("%s", str);
   } else {
      if (!save_admin_config(lbs->name, p, str))
         rsprintf("%s", str);
   }

   xfree(buffer);
}