void show_logbook_rename(LOGBOOK *lbs) {
   int i;
   char str[256], lbn[256];

   if (isparam("lbname")) {

      /* check if logbook name exists already */
      strcpy(lbn, getparam("lbname"));
      for (i = 0; lb_list[i].name[0]; i++)
         if (strieq(lbn, lb_list[i].name)) {
            sprintf(str, loc("Logbook \"%s\" exists already, please choose different name"), lbn);
            show_error(str);
            return;
         }

      if (!rename_logbook(lbs, getparam("lbname")))
         return;

      sprintf(str, "../%s/?cmd=Config", getparam("lbname"));
      redirect(NULL, str);
      return;

   } else {

      strcpy(str, loc("Rename logbook"));
      show_standard_header(lbs, TRUE, str, "", FALSE, NULL, NULL, 0);

      rsprintf("<table class=\"dlgframe\" cellspacing=0 align=center>");
      rsprintf("<tr><td class=\"dlgtitle\">\n");

      /* define hidden field for command */
      rsprintf("<input type=hidden name=cmd value=\"%s\">\n", loc("Rename this logbook"));

      rsprintf("%s</td></tr>\n", loc("Enter new logbook name"));

      rsprintf("<tr><td align=center class=\"dlgform\">");
      rsprintf("<input type=text name=\"lbname\"><br><br>\n");
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Rename this logbook"));
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Cancel"));
      rsprintf("</td></tr>\n\n");
   }

   rsprintf("</table>\n");
   show_bottom_text(lbs);
   rsprintf("</body></html>\r\n");
}