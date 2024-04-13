void receive_pwdfile(LOGBOOK *lbs, char *server, char *error_str) {
   char str[256], url[256], pwd[256], *buffer, *buf, *p;
   int i, status, version, fh, ssl;

   error_str[0] = 0;

   do {

      combine_url(lbs, server, "", url, sizeof(url), &ssl);
      strlcpy(str, url, sizeof(str));
      strcat(str, "?cmd=GetPwdFile");   // request password file

      if (retrieve_url(lbs, str, ssl, &buffer, TRUE) < 0) {
         *strchr(str, '?') = 0;
         sprintf(error_str, "Cannot contact elogd server at http://%s", str);
         return;
      }

      /* check version */
      p = strstr(buffer, "ELOG HTTP ");
      if (!p) {
         sprintf(error_str, "Remote server is not an ELOG server");
         xfree(buffer);
         return;
      }
      version = atoi(p + 10) * 100 + atoi(p + 12) * 10 + atoi(p + 14);
      if (version < 254) {
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
         xfree(buffer);
         *strchr(str, '?') = 0;
         sprintf(error_str, "Received invalid response from elogd server at http://%s", str);
         xfree(buffer);
         return;
      }
      p++;
      status = atoi(p);
      if (status != 200 && status != 302 && status != 404) {
         xfree(buffer);
         *strchr(str, '?') = 0;
         sprintf(error_str, "Received invalid response from elogd server at http://%s", str);
         return;
      }

      p = strstr(buffer, "\r\n\r\n");
      if (p == NULL) {
         xfree(buffer);
         sprintf(error_str, loc("Cannot receive \"%s\""), str);
         return;
      }
      p += 4;

      /* check for logbook access */
      if (strstr(p, loc("Please login")) || strstr(p, "GetPwdFile") || status == 302) {

         if (strstr(buffer, "?fail="))
            eprintf("\nInvalid username or password.");

         if (strstr(p, loc("Please login")) == NULL && strstr(p, "GetPwdFile") && isparam("unm"))
            eprintf("\nUser \"%s\" has no admin rights on remote server.", getparam("unm"));

         /* ask for username and password */
         eprintf("\nPlease enter admin username to access\n%s: ", url);
         fgets(str, sizeof(str), stdin);
         while (str[strlen(str) - 1] == '\r' || str[strlen(str) - 1] == '\n')
            str[strlen(str) - 1] = 0;
         setparam("unm", str);

         eprintf("\nPlease enter admin password to access\n%s: ", url);
         read_password(str, sizeof(str));
         eprintf("\n");
         while (str[strlen(str) - 1] == '\r' || str[strlen(str) - 1] == '\n')
            str[strlen(str) - 1] = 0;
         do_crypt(str, pwd, sizeof(pwd));
         setparam("upwd", pwd);
         status = 0;
      }

   } while (status != 200);

   get_password_file(lbs, str, sizeof(str));
   fh = open(str, O_CREAT | O_RDWR | O_BINARY, 0644);
   if (fh < 0) {
      sprintf(error_str, loc("Cannot open file <b>%s</b>"), str);
      strcat(error_str, ": ");
      strcat(error_str, strerror(errno));
      return;
   }

   buf = (char *) xmalloc(2 * strlen(p));
   strlcpy(buf, p, 2 * strlen(p));
   adjust_crlf(buf, 2 * strlen(p));

   i = write(fh, buf, strlen(buf));
   if (i < (int) strlen(buf)) {
      sprintf(error_str, loc("Cannot write to <b>%s</b>"), str);
      strcat(error_str, ": ");
      strcat(error_str, strerror(errno));
      close(fh);
      xfree(buf);
      xfree(buffer);
      return;
   }

   TRUNCATE(fh);

   close(fh);

   xfree(buf);
   xfree(buffer);
}