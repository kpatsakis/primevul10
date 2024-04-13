void show_rss_feed(LOGBOOK *lbs) {
   int i, n, size, index, message_id, offset;
   char str[256], charset[256], url[256], attrib[MAX_N_ATTR][NAME_LENGTH], date[80], *text, title[2000],
           slist[MAX_N_ATTR + 10][NAME_LENGTH], svalue[MAX_N_ATTR + 10][NAME_LENGTH], draft[1000];
   time_t ltime;
   struct tm *ts;

   rsprintf("HTTP/1.1 200 Document follows\r\n");
   rsprintf("Server: ELOG HTTP %s-%s\r\n", VERSION, git_revision());

   if (!getcfg("global", "charset", charset, sizeof(charset)))
      strcpy(charset, DEFAULT_HTTP_CHARSET);

   rsprintf("Content-Type: text/xml;charset=%s\r\n", charset);
   rsprintf("\r\n");

   rsprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n", charset);
   rsprintf("<rss version=\"2.0\">\n");
   rsprintf("<channel>\n");

   rsprintf("<title>ELOG %s</title>\n", lbs->name);

   /* retrive URL */
   getcfg(lbs->name, "URL", url, sizeof(url));

   /* if HTTP request comes from localhost, use localhost as
      absolute link (needed if running on DSL at home) */
   if (!url[0] && strstr(http_host, "localhost")) {
      if (_ssl_flag)
         strcpy(url, "https://localhost");
      else
         strcpy(url, "http://localhost");
      if (elog_tcp_port != 80)
         sprintf(url + strlen(url), ":%d", elog_tcp_port);
      strcat(url, "/");
   }

   if (!url[0]) {
      /* assemble absolute path from host name and port */
      if (_ssl_flag)
         sprintf(url, "https://%s", host_name);
      else
         sprintf(url, "http://%s", host_name);
      if (elog_tcp_port != 80)
         sprintf(url + strlen(url), ":%d", elog_tcp_port);
      strcat(url, "/");
   }

   /* add trailing '/' if not present */
   if (url[strlen(url) - 1] != '/')
      strcat(url, "/");

   strlcat(url, lbs->name_enc, sizeof(url));

   rsprintf("<link>%s</link>\n", url);

   if (getcfg(lbs->name, "Comment", str, sizeof(str))) {
      rsprintf("<description>");
      xmlencode(str);
      rsprintf("</description>\n");
   }

   rsprintf("<generator>ELOG V%s</generator>\n", VERSION);

   rsprintf("<image>\n");
   rsprintf("<url>%s/elog.png</url>\n", url);
   rsprintf("<title>ELOG %s</title>\n", lbs->name_enc);
   rsprintf("<link>%s</link>\n", url);
   rsprintf("</image>\n");

   /*---- show last <n> items ----*/

   n = 15;
   if (getcfg(lbs->name, "RSS Entries", str, sizeof(str)))
      n = atoi(str);

   text = (char *) xmalloc(TEXT_SIZE);
   message_id = el_search_message(lbs, EL_LAST, 0, FALSE);
   for (index = 0; index < n && message_id; index++) {

      size = TEXT_SIZE;
      el_retrieve(lbs, message_id, date, attr_list, attrib, lbs->n_attr, text, &size, NULL, NULL,
                  NULL, NULL, NULL, draft);

      /* skip drafts */
      if (draft[0]) {
         message_id = el_search_message(lbs, EL_PREV, message_id, FALSE);
         continue;
      }

      /* limit text size to 2k */
      text[2048] = 0;

      if (getcfg(lbs->name, "RSS Title", title, sizeof(title))) {
         i = build_subst_list(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, attrib,
                              TRUE);
         sprintf(str, "%d", message_id);
         add_subst_list((char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, "message id", str, &i);
         add_subst_time(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, "entry time",
                        date, &i, 0);

         strsubst_list(title, sizeof(title), (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue,
                       i);
      } else {

         title[0] = 0;
         for (i = 0; i < lbs->n_attr; i++) {
            if (attrib[i][0]) {
               strlcat(title, attrib[i], sizeof(title));
               if (i < lbs->n_attr - 1)
                  strlcat(title, ", ", sizeof(title));
            }
         }

      }

      rsprintf("<item>\n");

      /* convert date to RFC-822 date */
      setlocale(LC_ALL, "C");
      ltime = date_to_ltime(date);
      ts = localtime(&ltime);
      assert(ts);
      strftime(str, sizeof(str), "%a, %d %b %Y %H:%M:%S", ts);
      offset = (-(int) my_timezone());
      if (ts->tm_isdst)
         offset += 3600;
      snprintf(date, sizeof(date) - 1, "%s %+03d%02d", str, (int) (offset / 3600), (int) ((abs((int) offset)
                                                                                           / 60) % 60));
      getcfg("global", "Language", str, sizeof(str));
      if (str[0])
         setlocale(LC_ALL, str);

      rsprintf("<title>");
      xmlencode(title);
      rsprintf("</title>\n");

      rsprintf("<link>");
      strlcpy(str, url, sizeof(str));
      sprintf(str + strlen(str), "/%d", message_id);
      xmlencode(str);
      rsprintf("</link>\n");

      rsprintf("<description>\n");
      xmlencode(text);
      rsprintf("</description>\n");

      rsprintf("<pubDate>\n");
      rsprintf("%s", date);
      rsprintf("</pubDate>\n");

      rsprintf("</item>\n");
      message_id = el_search_message(lbs, EL_PREV, message_id, FALSE);
   }

   xfree(text);
   rsprintf("</channel>\n");
   rsprintf("</rss>\n");
}