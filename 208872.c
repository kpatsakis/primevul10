void show_error(char *error) {
   char str[256];

   /* header */
   rsprintf("HTTP/1.1 404 Not Found\r\n");
   rsprintf("Server: ELOG HTTP %s-%s\r\n", VERSION, git_revision());
   if (getcfg("global", "charset", str, sizeof(str)))
      rsprintf("Content-Type: text/html;charset=%s\r\n", str);
   else
      rsprintf("Content-Type: text/html;charset=%s\r\n", DEFAULT_HTTP_CHARSET);
   rsprintf("\r\n");

   rsprintf("<!DOCTYPE html>\n");
   rsprintf("<html><head>\n");
   rsprintf("<META NAME=\"ROBOTS\" CONTENT=\"NOINDEX, NOFOLLOW\">\n");
   rsprintf("<title>%s</title>\n", loc("ELOG error"));
   rsprintf("<link rel=\"stylesheet\" type=\"text/css\" href=\"elog.css\">\n");
   rsprintf("</head>\n");

   rsprintf("<body><center>\n");
   rsprintf("<table class=\"dlgframe\" width=\"50%%\" cellpadding=\"1\" cellspacing=\"0\"");
   rsprintf("<tr><td class=\"errormsg\">%s</td></tr>\n", error);

   rsprintf("<tr><td class=\"errormsg\">");

   rsprintf("<script language=\"javascript\" type=\"text/javascript\">\n");
   rsprintf("document.write(\"<button type=button onClick=history.back()>%s</button>\"); \n", loc("Back"));
   rsprintf("</script>\n");

   rsprintf("<noscript>\n");
   rsprintf("%s\n", loc("Please use your browser's back button to go back"));
   rsprintf("</noscript>\n");

   rsprintf("</td></tr>\n</table>\n");
   rsprintf("</center></body></html>\n");
}