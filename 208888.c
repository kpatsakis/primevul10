int retrieve_url(LOGBOOK *lbs, const char *url, int ssl, char **buffer, BOOL send_unm) {
   char str[1000], unm[256], upwd[256], host[256], subdir[256], param[256];
   int port, bufsize;
   int i, n;
   fd_set readfds;
   struct timeval timeout;

   UNUSED(ssl);

#ifdef HAVE_SSL
   SSL *ssl_con = NULL;
#else
   void *ssl_con = NULL;
#endif
   int sock;

   *buffer = NULL;
   split_url(url, host, &port, subdir, param);

   /* create a new socket for connecting to remote server */
   sock = elog_connect(host, port);
   if (sock == -1)
      return -1;
#ifdef HAVE_SSL
   if (ssl)
      if (ssl_connect(sock, &ssl_con) < 0) {
         printf("Error initiating SSL connection\n");
         return -1;
      }
#endif

   /* compose GET request, avoid chunked data in HTTP/1.1 protocol */
   sprintf(str, "GET %s%s HTTP/1.0\r\nConnection: Close\r\n", subdir, param);

   /* add local username/password */
   if (send_unm) {
      if (isparam("unm")) {
         strlcpy(unm, getparam("unm"), sizeof(unm));
         if (isparam("upwd"))
            strlcpy(upwd, getparam("upwd"), sizeof(upwd));
         else
            get_user_line(lbs, getparam("unm"), upwd, NULL, NULL, NULL, NULL, NULL);

         sprintf(str + strlen(str), "Cookie: unm=%s; upwd=%s\r\n", unm, upwd);
      }
   }

   /* add host (RFC2616, Sec. 14) */
   sprintf(str + strlen(str), "Host: %s:%d\r\n", host, port);

   strcat(str, "\r\n");

   send_with_timeout(ssl_con, sock, (char *) str, strlen(str));

   bufsize = TEXT_SIZE + 1000;
   *buffer = xmalloc(bufsize);
   memset(*buffer, 0, bufsize);

   n = 0;

   do {
      FD_ZERO(&readfds);
      FD_SET(sock, &readfds);

      timeout.tv_sec = 30;      /* 30 sec. timeout */
      timeout.tv_usec = 0;

      select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);

      if (!FD_ISSET(sock, &readfds)) {
         closesocket(sock);
         sock = 0;
         xfree(*buffer);
         *buffer = NULL;
         return -1;
      }
#ifdef HAVE_SSL
      if (ssl)
         i = SSL_read(ssl_con, *buffer + n, bufsize - n);
      else
#endif
         i = recv(sock, *buffer + n, bufsize - n, 0);

      if (i <= 0)
         break;

      n += i;

      if (n >= bufsize) {
         /* increase buffer size */
         bufsize += 10000;
         *buffer = (char *) xrealloc(*buffer, bufsize);
         if (*buffer == NULL) {
            closesocket(sock);
            return -1;
         }
      }

   } while (1);

   return n;
}