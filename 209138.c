void show_query(LOGBOOK *lbs, char *title, char *query_string, char *button1, char *button1_url,
                char *button2, char *button2_url) {
   show_standard_header(lbs, TRUE, "ELog query", title, FALSE, NULL, NULL, 0);

   rsprintf("<table class=\"dlgframe\" cellspacing=0 align=center>");

   rsprintf("<tr><td class=\"dlgtitle\">\n");
   rsprintf("%s</td></tr>\n", title);

   rsprintf("<tr><td align=center class=\"dlgform\">");
   rsprintf("%s", query_string);
   rsprintf("</td></tr>\n\n");

   rsprintf("<tr><td align=center class=\"dlgform\">");
   rsprintf("<input type=button value=\"%s\" onClick=\"window.location.href='%s';\">\n", button1,
            button1_url);
   rsprintf("<input type=button value=\"%s\" onClick=\"window.location.href='%s';\">\n", button2,
            button2_url);
   rsprintf("</td></tr>\n\n");

   rsprintf("</table>\n");
   show_bottom_text(lbs);
   rsprintf("</body></html>\r\n");
}