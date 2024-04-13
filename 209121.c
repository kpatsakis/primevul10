void show_standard_header(LOGBOOK *lbs, BOOL expires, char *title, char *path, BOOL rss_feed, char *cookie,
                          char *script, int refresh) {
   if (script) {
      show_html_header(lbs, expires, title, FALSE, rss_feed, cookie, FALSE, refresh);

      rsprintf("<script type=\"text/javascript\">\n");
      rsprintf("<!--\n");
      show_browser(browser);

      rsprintf("logbook = \"%s\";\n", lbs->name_enc);
      rsprintf("//-->\n");
      rsprintf("</script>\n");
      rsprintf("<script type=\"text/javascript\" src=\"../elcode.js\"></script>\n\n");
      rsprintf("</head>\n");
   } else
      show_html_header(lbs, expires, title, TRUE, rss_feed, cookie, FALSE, refresh);

   if (script)
      rsprintf("<body %s>\n", script);
   else
      rsprintf("<body>\n");

   show_top_text(lbs);

   if (path && path[0])
      rsprintf("<form name=\"form1\" method=\"GET\" action=\"%s\">\n\n", path);
   else
      rsprintf("<form name=\"form1\" method=\"GET\" action=\".\">\n\n");
}