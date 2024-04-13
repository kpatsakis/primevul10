void show_logbook_new(LOGBOOK *lbs) {
   char str[256], lbn[256];
   int i;

   if (isparam("lbname")) {

      /* check if logbook name exists already */
      strcpy(lbn, getparam("lbname"));
      for (i = 0; lb_list[i].name[0]; i++)
         if (strieq(lbn, lb_list[i].name)) {
            sprintf(str, loc("Logbook \"%s\" exists already, please choose different name"), lbn);
            show_error(str);
            return;
         }

      /* create new logbook */
      if (!create_logbook(lbs, getparam("lbname"), getparam("template")))
         return;

      strcpy(lbn, getparam("lbname"));
      url_encode(lbn, sizeof(lbn));
      sprintf(str, "../%s/?cmd=Config", lbn);
      for (i = 0; lb_list[i].name[0]; i++)
         if (strieq(lbn, lb_list[i].name))
            break;
      if (lb_list[i].name[0])
         redirect(&lb_list[i], str);
      else
         redirect(NULL, str);
      return;
   }

   show_standard_header(lbs, TRUE, loc("Create new logbook"), "", FALSE, NULL, NULL, 0);

   rsprintf("<table class=\"dlgframe\" cellspacing=0 align=center>");
   rsprintf("<tr><td class=\"dlgtitle\">\n");

   /* define hidden field for command */
   rsprintf("<input type=hidden name=cmd value=\"%s\">\n", loc("Create new logbook"));
   rsprintf("%s</td></tr>\n", loc("Create new logbook"));

   rsprintf("<tr><td align=center class=\"dlgform\">\n");
   rsprintf("%s :&nbsp;&nbsp;", loc("Logbook name"));
   rsprintf("<input type=text name=lbname>\n");
   rsprintf("</td></tr>\n");

   rsprintf("<tr><td align=center class=\"dlgform\">\n");
   rsprintf("%s : \n", loc("Use existing logbook as template"));
   rsprintf("<select name=template>\n");
   rsprintf("<option value=\"\">- %s -\n", loc("none"));
   for (i = 0; lb_list[i].name[0]; i++)
      rsprintf("<option value=\"%s\">%s\n", lb_list[i].name, lb_list[i].name);
   rsprintf("</select>\n");
   rsprintf("</td></tr>\n\n");

   rsprintf("<tr><td align=center class=\"dlgform\">\n");
   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Create new logbook"));
   rsprintf("<input type=submit name=tmp value=\"%s\">\n", loc("Cancel"));
   rsprintf("</td></tr>\n\n");

   rsprintf("</table>\n");
   show_bottom_text(lbs);
   rsprintf("</body></html>\r\n");
}