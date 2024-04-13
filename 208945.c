int retrieve_remote_md5(LOGBOOK *lbs, char *host, MD5_INDEX **md5_index, char *error_str) {
   int i, n, id, x, version, ssl;
   char *text, *p, url[256], str[1000];

   *md5_index = NULL;

   combine_url(lbs, host, "?cmd=GetMD5", url, sizeof(url), &ssl);

   text = NULL;
   error_str[0] = 0;
   if (retrieve_url(lbs, url, ssl, &text, TRUE) < 0) {
      sprintf(error_str, loc("Cannot connect to remote server \"%s\""), host);
      return -1;
   }
   p = strstr(text, "ELOG HTTP ");
   if (!p) {
      if (isparam("debug"))
         rsputs(text);
      strlcpy(error_str, loc("Remote server is not an ELOG server"), 256);
      xfree(text);
      return -1;
   }
   version = atoi(p + 10) * 100 + atoi(p + 12) * 10 + atoi(p + 14);
   if (version < 250) {
      if (isparam("debug"))
         rsputs(text);
      memset(str, 0, sizeof(str));
      strncpy(str, p + 10, 5);
      sprintf(error_str, loc("Incorrect remote ELOG server version %s"), str);
      xfree(text);
      return -1;
   }

   p = strstr(text, "Location: ");
   if (p) {
      if (isparam("debug"))
         rsputs(text);

      if (strstr(text, "?fail="))
         sprintf(error_str, loc("Invalid user name \"%s\" or password for remote logbook"),
                 isparam("unm") ? getparam("unm") : "");
      else {
         strlcpy(str, p + 9, sizeof(str));
         if (strchr(str, '?'))
            *strchr(str, '?') = 0;
         strlcpy(error_str, loc("URL is redirected to:"), 256);
         strlcat(error_str, str, 256);
      }

      return -3;
   }

   p = strstr(text, "\r\n\r\n");
   if (!p) {
      if (isparam("debug"))
         rsputs(text);
      strlcpy(error_str, loc("Invalid HTTP header"), 256);
      xfree(text);
      return -1;
   }

   for (n = 0;; n++) {
      p = strstr(p, "ID:");
      if (!p)
         break;
      p += 3;

      id = atoi(p);

      p = strstr(p, "MD5:");
      if (!p)
         break;
      p += 4;

      if (n == 0)
         *md5_index = (MD5_INDEX *) xmalloc(sizeof(MD5_INDEX));
      else
         *md5_index = (MD5_INDEX *) xrealloc(*md5_index, (n + 1) * sizeof(MD5_INDEX));

      (*md5_index)[n].message_id = id;

      for (i = 0; i < 16; i++) {
         sscanf(p + 2 * i, "%02X", &x);
         (*md5_index)[n].md5_digest[i] = (unsigned char) x;
      }
   }

   if (n == 0) {
      if (isparam("debug"))
         rsputs(text);
      if (strstr(text, "Login")) {
         strlcpy(error_str, loc("No user name supplied to access remote logbook"), 256);
         xfree(text);
         return -2;
      } else
         strlcpy(error_str, loc("Error accessing remote logbook"), 256);
   }

   xfree(text);

   return n;
}