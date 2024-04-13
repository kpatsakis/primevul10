void submit_config(LOGBOOK *lbs, char *server, char *buffer, char *error_str) {
   int i, n, port, sock, content_length, header_length, ssl;
   char str[256], upwd[80];
   char subdir[256], param[256], remote_host_name[256];
   char *content, boundary[80], request[10000], response[10000];
#ifdef HAVE_SSL
   SSL *ssl_con = NULL;
#else
   void *ssl_con = NULL;
#endif

   error_str[0] = 0;

   combine_url(lbs, server, "", str, sizeof(str), &ssl);
   split_url(str, remote_host_name, &port, subdir, param);

   sock = elog_connect(remote_host_name, port);
   if (sock == -1) {
      sprintf(error_str, loc("Cannot connect to host %s, port %d"), remote_host_name, port);
      return;
   }
#ifdef HAVE_SSL
   if (ssl)
      if (ssl_connect(sock, &ssl_con) < 0) {
         strlcpy(error_str, "Error initiating SSL connection\n", 256);
         return;
      }
#endif

   content_length = 100000;
   content = (char *) xmalloc(content_length);

   /* compose content */
   sprintf(boundary, "---------------------------%04X%04X%04X", rand(), rand(), rand());
   strcpy(content, boundary);
   strcat(content, "\r\nContent-Disposition: form-data; name=\"cmd\"\r\n\r\nSave\r\n");

   if (isparam("unm")) {
      sprintf(content + strlen(content), "%s\r\nContent-Disposition: form-data; name=\"unm\"\r\n\r\n%s\r\n",
              boundary, getparam("unm"));

      if (isparam("upwd"))
         strlcpy(upwd, getparam("upwd"), sizeof(upwd));
      else
         get_user_line(lbs, getparam("unm"), upwd, NULL, NULL, NULL, NULL, NULL);
      sprintf(content + strlen(content), "%s\r\nContent-Disposition: form-data; name=\"upwd\"\r\n\r\n%s\r\n",
              boundary, upwd);
   }

   sprintf(content + strlen(content),
           "%s\r\nContent-Disposition: form-data; name=\"Text\"\r\n\r\n%s\r\n%s\r\n", boundary, buffer,
           boundary);

   content_length = strlen(content);

   /* compose request */
   strcpy(request, "POST ");
   if (subdir[0]) {
      if (subdir[0] != '/')
         strcat(request, "/");
      strcat(request, subdir);
      if (request[strlen(request) - 1] != '/')
         strcat(request, "/");
   }
   strcat(request, " HTTP/1.0\r\n");

   sprintf(request + strlen(request), "Content-Type: multipart/form-data; boundary=%s\r\n", boundary);
   sprintf(request + strlen(request), "Host: %s\r\n", host_name);
   sprintf(request + strlen(request), "User-Agent: ELOGD\r\n");
   sprintf(request + strlen(request), "Content-Length: %d\r\n", content_length);

   if (isparam("wpwd"))
      sprintf(request + strlen(request), "Cookie: wpwd=%s\r\n", getparam("wpwd"));

   strcat(request, "\r\n");

   header_length = strlen(request);

   send_with_timeout(ssl_con, sock, request, header_length);
   send_with_timeout(ssl_con, sock, content, content_length);


#ifdef HAVE_SSL
   if (ssl)
      /* receive response */
      i = SSL_read(ssl_con, response, 10000);
   else
#endif
      /* receive response */
      i = recv(sock, response, 10000, 0);

   if (i < 0) {
      closesocket(sock);
      strlcpy(error_str, "Cannot receive response", 256);
      return;
   }

   /* discard remainder of response */
   n = i;
   while (i > 0) {
      i = recv(sock, response + n, 10000, 0);
      if (i > 0)
         n += i;
   }
   response[n] = 0;

#ifdef HAVE_SSL
   if (ssl) {
      SSL_shutdown(ssl_con);
      SSL_free(ssl_con);
   }
#endif

   closesocket(sock);

   /* check response status */
   if (strstr(response, "302 Found")) {
      if (strstr(response, "Location:")) {
         if (strstr(response, "fail"))
            strlcpy(error_str, "Invalid usr name or password\n", 256);
      }
   } else if (strstr(response, "Logbook Selection"))
      strlcpy(error_str, "No logbook specified\n", 256);
   else if (strstr(response, "enter password"))
      strlcpy(error_str, "Missing or invalid password\n", 256);
   else if (strstr(response, "form name=form1"))
      strlcpy(error_str, "Missing or invalid user name/password\n", 256);
   else if (strstr(response, "Error: Attribute")) {
      strncpy(str, strstr(response, "Error: Attribute") + 20, sizeof(str));
      if (strchr(str, '<'))
         *strchr(str, '<') = 0;
      sprintf(error_str, "Missing required attribute \"%s\"\n", str);
   } else
      strlcpy(error_str, "Error transmitting message\n", 256);
}