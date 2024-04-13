int submit_message(LOGBOOK *lbs, char *host, int message_id, char *error_str) {
   int size, i, n, status, fh, port, sock, content_length, header_length, remote_id, n_attr, ssl;
   char str[256], file_name[MAX_PATH_LENGTH], attrib[MAX_N_ATTR][NAME_LENGTH];
   char subdir[256], param[256], remote_host_name[256], url[256], upwd[80];
   char date[80], *text, in_reply_to[80], reply_to[MAX_REPLY_TO * 10],
           attachment[MAX_ATTACHMENTS][MAX_PATH_LENGTH], encoding[80], locked_by[256], draft[256], *buffer;
   char *content, *p, boundary[80], request[10000], response[10000];
#ifdef HAVE_SSL
   SSL *ssl_con = NULL;
#else
   void *ssl_con = NULL;
#endif

   text = (char *) xmalloc(TEXT_SIZE);
   error_str[0] = 0;

   /* get message with attribute list devied from database */
   size = TEXT_SIZE;
   status = el_retrieve(lbs, message_id, date, attr_list, attrib, -1, text, &size, in_reply_to, reply_to,
                        attachment, encoding, locked_by, draft);

   if (status != EL_SUCCESS) {
      xfree(text);
      strcpy(error_str, loc("Cannot read entry from local logbook"));
      return -1;
   }

   /* count attributes */
   for (n_attr = 0; attr_list[n_attr][0]; n_attr++);

   combine_url(lbs, host, "", url, sizeof(url), &ssl);
   split_url(url, remote_host_name, &port, subdir, param);

   sock = elog_connect(remote_host_name, port);
   if (sock == -1) {
      sprintf(error_str, loc("Cannot connect to host %s, port %d"), remote_host_name, port);
      return -1;
   }
#ifdef HAVE_SSL
   if (ssl)
      if (ssl_connect(sock, &ssl_con) < 0) {
         strcpy(error_str, "Error initiating SSL connection\n");
         return -1;
      }
#endif

   content_length = 100000;
   for (i = 0; i < MAX_ATTACHMENTS; i++)
      if (attachment[i][0]) {
         strlcpy(file_name, lbs->data_dir, sizeof(file_name));
         generate_subdir_name(attachment[i], subdir, sizeof(subdir));
         strlcat(file_name, subdir, sizeof(file_name));
         strlcat(file_name, attachment[i], sizeof(file_name));

         fh = open(file_name, O_RDONLY | O_BINARY);
         if (fh > 0) {
            lseek(fh, 0, SEEK_END);
            size = TELL(fh);
            close(fh);
         } else
            size = 0;

         content_length += size;
      }

   content = (char *) xmalloc(content_length);

   /* compose content */
   sprintf(boundary, "---------------------------%04X%04X%04X", rand(), rand(), rand());
   strcpy(content, boundary);
   strcat(content, "\r\nContent-Disposition: form-data; name=\"cmd\"\r\n\r\nSubmit\r\n");

   sprintf(content + strlen(content),
           "%s\r\nContent-Disposition: form-data; name=\"mirror_id\"\r\n\r\n%d\r\n", boundary, message_id);

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

   if (in_reply_to[0])
      sprintf(content + strlen(content),
              "%s\r\nContent-Disposition: form-data; name=\"in_reply_to\"\r\n\r\n%s\r\n", boundary,
              in_reply_to);

   if (reply_to[0])
      sprintf(content + strlen(content),
              "%s\r\nContent-Disposition: form-data; name=\"reply_to\"\r\n\r\n%s\r\n", boundary, reply_to);

   for (i = 0; i < n_attr; i++)
      sprintf(content + strlen(content), "%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n",
              boundary, attr_list[i], attrib[i]);

   sprintf(content + strlen(content),
           "%s\r\nContent-Disposition: form-data; name=\"entry_date\"\r\n\r\n%s\r\n", boundary, date);

   sprintf(content + strlen(content),
           "%s\r\nContent-Disposition: form-data; name=\"encoding\"\r\n\r\n%s\r\n", boundary, encoding);

   sprintf(content + strlen(content),
           "%s\r\nContent-Disposition: form-data; name=\"Text\"\r\n\r\n%s\r\n%s\r\n", boundary, text,
           boundary);

   content_length = strlen(content);
   p = content + content_length;

   /* read attachments */
   for (i = 0; i < MAX_ATTACHMENTS; i++)
      if (attachment[i][0]) {
         strlcpy(file_name, lbs->data_dir, sizeof(file_name));
         generate_subdir_name(attachment[i], subdir, sizeof(subdir));
         strlcat(file_name, subdir, sizeof(file_name));
         strlcat(file_name, attachment[i], sizeof(file_name));

         fh = open(file_name, O_RDONLY | O_BINARY);
         if (fh > 0) {
            lseek(fh, 0, SEEK_END);
            size = TELL(fh);
            lseek(fh, 0, SEEK_SET);
            buffer = (char *) xmalloc(size);
            read(fh, buffer, size);
            close(fh);

            /* submit attachment */
            sprintf(p, "Content-Disposition: form-data; name=\"attfile%d\"; filename=\"%s\"\r\n\r\n", i + 1,
                    attachment[i]);

            content_length += strlen(p);
            p += strlen(p);

            memcpy(p, buffer, size);
            p += size;
            strcpy(p, boundary);
            strcat(p, "\r\n");

            content_length += size + strlen(p);
            p += strlen(p);

            xfree(buffer);
         }
      }

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
      xfree(text);
      strcpy(error_str, "Cannot receive response");
      return -1;
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
   remote_id = -1;

   /* check response status */
   if (strstr(response, "302 Found")) {
      if (strstr(response, "Location:")) {
         if (strstr(response, "fail"))
            sprintf(error_str, "Invalid user name or password\n");

         strlcpy(str, strstr(response, "Location:") + 9, sizeof(str));
         if (strchr(str, '\n'))
            *strchr(str, '\n') = 0;
         if (strchr(str, '?'))
            *strchr(str, '?') = 0;

         if (strrchr(str, '/'))
            remote_id = atoi(strrchr(str, '/') + 1);
         else
            remote_id = atoi(str);
      }
   } else if (strstr(response, "Logbook Selection"))
      sprintf(error_str, "No logbook specified\n");
   else if (strstr(response, "enter password"))
      sprintf(error_str, "Missing or invalid password\n");
   else if (strstr(response, "form name=form1"))
      sprintf(error_str, "Missing or invalid user name/password\n");
   else if (strstr(response, "Error: Attribute")) {
      strncpy(str, strstr(response, "Error: Attribute") + 20, sizeof(str));
      if (strchr(str, '<'))
         *strchr(str, '<') = 0;
      sprintf(error_str, "Missing required attribute \"%s\"\n", str);
   } else
      sprintf(error_str, "Error transmitting message\n");

   if (error_str[0] && isparam("debug"))
      rsputs(response);

   xfree(text);

   if (error_str[0])
      return -1;

   return remote_id;
}