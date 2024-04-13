void show_logbook_delete(LOGBOOK *lbs) {
   char str[256];

   /* redirect if confirm = NO */
   if (isparam("confirm") && strcmp(getparam("confirm"), loc("No")) == 0) {

      redirect(lbs, "?cmd=Config");
      return;
   }

   if (isparam("confirm")) {
      if (strcmp(getparam("confirm"), loc("Yes")) == 0) {

         /* delete logbook */
         str[0] = 0;
         delete_logbook(lbs, str);
         if (str[0])
            show_error(str);
         else
            redirect(NULL, "../");
         return;
      }

   } else {

      strcpy(str, "Delete logbook");
      show_standard_header(lbs, TRUE, str, "", FALSE, NULL, NULL, 0);

      rsprintf("<table class=\"dlgframe\" cellspacing=0 align=center>");
      rsprintf("<tr><td class=\"dlgtitle\">\n");

      /* define hidden field for command */
      rsprintf("<input type=hidden name=cmd value=\"%s\">\n", loc("Delete this logbook"));

      sprintf(str, loc("Are you sure to delete logbook \"%s\"?"), lbs->name);
      rsprintf("%s</td></tr>\n", str);

      rsprintf("<tr><td align=center class=\"dlgform\">");
      rsprintf("<input type=submit name=confirm value=\"%s\">\n", loc("Yes"));
      rsprintf("<input type=submit name=confirm value=\"%s\">\n", loc("No"));
      rsprintf("</td></tr>\n\n");
   }

   rsprintf("</table>\n");
   show_bottom_text(lbs);
   rsprintf("</body></html>\r\n");
}