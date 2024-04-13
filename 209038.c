void show_html_header(LOGBOOK *lbs, BOOL expires, char *title, BOOL close_head, BOOL rss_feed, char *cookie,
                      int embed_css, int refresh) {
   int i, n;
   char css[1000], css_base[1000], str[1000], media[1000], file_name[256];
   char css_list[MAX_N_LIST][NAME_LENGTH];

   show_http_header(lbs, expires, cookie);

   /* DOCTYPE */
   rsprintf("<!DOCTYPE html>\n");

   /* this code would be for XML files...
      rsprintf("<?xml-stylesheet type=\"text/xsl\" href=\"http://www.w3.org/Math/XSL/mathml.xsl\"?>\n");
      rsprintf("<html xmlns=\"http://www.w3.org/1999/xhtml\"><head>\n");
    */

   /* page title */
   rsprintf("<html><head>\n");
   rsprintf("<meta name=\"ROBOTS\" content=\"NOINDEX, NOFOLLOW\">\n");
   if (refresh)
      rsprintf("<meta http-equiv=\"refresh\" content=\"%d\">\n", refresh);
   rsprintf("<title>%s</title>\n", title);

   /* Cascading Style Sheet */
   if (embed_css) {
      strlcpy(css, "elog.css", sizeof(css));
      if (lbs != NULL && getcfg(lbs->name, "CSS", str, sizeof(str)))
         strlcpy(css, str, sizeof(css));
      else if (lbs == NULL && getcfg("global", "CSS", str, sizeof(str)))
         strlcpy(css, str, sizeof(css));

      strlcpy(file_name, resource_dir, sizeof(file_name));
      strlcat(file_name, "themes", sizeof(file_name));
      strlcat(file_name, DIR_SEPARATOR_STR, sizeof(file_name));
      strlcat(file_name, theme_name, sizeof(file_name));
      strlcat(file_name, DIR_SEPARATOR_STR, sizeof(file_name));
      strlcat(file_name, css, sizeof(file_name));

      FILE *f = fopen(file_name, "rb");
      if (f != NULL) {
         fseek(f, 0, SEEK_END);
         int size = TELL(fileno(f));
         fseek(f, 0, SEEK_SET);

         char *buf = xmalloc(size + 100);
         fread(buf, 1, size, f);
         buf[size] = 0;
         fclose(f);

         rsprintf("<style>\n");
         rsputs(buf);
         rsprintf("</style>\n");

         xfree(buf);
      }
   } else {
      rsprintf("<link rel=\"stylesheet\" type=\"text/css\" href=\"elog.css\">\n");
   }

   if (lbs != NULL && getcfg(lbs->name, "CSS", str, sizeof(str)))
      strlcpy(css, str, sizeof(css));
   else if (lbs == NULL && getcfg("global", "CSS", str, sizeof(str)))
      strlcpy(css, str, sizeof(css));
   else
      css[0] = 0;

   if (css[0]) {
      if (strchr(css, ',')) {
         n = strbreak(css, css_list, MAX_N_LIST, ",", FALSE);
         for (i = 0; i < n; i++) {
            strlcpy(str, css_list[i], sizeof(str));
            if (strchr(str, '&')) {
               strlcpy(media, strchr(str, '&') + 1, sizeof(media));
               *strchr(str, '&') = 0;
               rsprintf("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\" media=\"%s\">\n", str, media);
            }
         }
      } else
         rsprintf("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s%s\">\n", css_base, css);
   }

   if (!embed_css) {
      rsprintf("<link rel=\"shortcut icon\" href=\"favicon.ico\" />\n");
      rsprintf("<link rel=\"icon\" href=\"favicon.png\" type=\"image/png\" />\n");
   }

   if (rss_feed && !embed_css) {
      rsprintf("<link rel=\"alternate\" type=\"application/rss+xml\" ");
      rsprintf("title=\"ELOG %s\" ", lbs->name);
      rsprintf("href=\"elog.rdf\" />\n");
   }

   if (close_head)
      rsprintf("</head>\n");
}