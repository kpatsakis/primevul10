void show_admin_page(LOGBOOK *lbs, char *top_group) {
   int rows, cols;
   char *buffer, error_str[256];
   char section[NAME_LENGTH], str[NAME_LENGTH], grp[NAME_LENGTH];

   /*---- header ----*/

   sprintf(str, "ELOG %s", loc("Admin"));
   show_html_header(lbs, FALSE, str, TRUE, FALSE, NULL, FALSE, 0);

   rsprintf("<body><form method=\"POST\" action=\"./\" enctype=\"multipart/form-data\">\n");

   /*---- title ----*/

   show_standard_title(lbs, "", 0);

   /*---- menu buttons ----*/

   rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");
   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Save"));
   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Cancel"));
   rsprintf("<input type=hidden name=cfgpage value=\"1\">\n");

   if (lbs->top_group[0] && (!top_group || strieq(top_group, "global"))) {
      if (is_admin_user(NULL, getparam("unm"))) {
         if (lbs->top_group[0]) {

            sprintf(str, "global %s", lbs->top_group);

            if (is_group(str)) {
               sprintf(grp, "[global %s]", lbs->top_group);
               sprintf(str, loc("Change %s"), grp);
               rsprintf("<input type=submit name=cmd value=\"%s\">\n", str);
            }
         }
      }
   }

   if (is_group("global") && !strieq(top_group, "global")) {
      if (is_admin_user_global(getparam("unm"))) {
         sprintf(str, loc("Change %s"), "[global]");
         rsprintf("<input type=submit name=cmd value=\"%s\">\n", str);
      }
   }

   if (top_group) {
      if (strieq(top_group, "global")) {
         rsprintf("<input type=hidden name=global value=\"global\">\n");
         strcpy(str, "[global]");
      } else {
         rsprintf("<input type=hidden name=global value=\"%s\">\n", top_group);
         sprintf(str, "[global %s]", top_group);
      }
      rsprintf("<br><center><b>%s</b></center>", str);
   }

   if (is_group("global") && !strieq(top_group, "global")) {
      if (is_admin_user(NULL, getparam("unm"))) {
         rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Delete this logbook"));
         rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Rename this logbook"));
         rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Create new logbook"));
      }
   }

   rsprintf("</span></td></tr>\n\n");

   /*---- entry form ----*/

   rsprintf("<tr><td class=\"form1\">\n");

   /* extract section of current logbook */
   if (top_group) {
      if (strieq(top_group, "global"))
         strcpy(section, "global");
      else
         sprintf(section, "global %s", top_group);
   } else
      strcpy(section, lbs->name);

   load_config_section(section, &buffer, error_str);

   if (error_str[0]) {
      rsprintf("<h2>%s</h2>\n", error_str);
      rsprintf("</table></td></tr></table>\n");
      rsprintf("</body></html>\r\n");
      return;
   }

   if (getcfg(section, "Admin textarea", str, sizeof(str)) && strchr(str, ',') != NULL) {
      cols = atoi(str);
      rows = atoi(strchr(str, ',') + 1);
   } else {
      cols = 120;
      rows = 30;
   }

   rsprintf("<textarea cols=%d rows=%d wrap=virtual name=Text>", cols, rows);

   rsputs3(buffer);
   xfree(buffer);

   rsprintf("</textarea>\n");

   /* put link for config page */
   rsprintf("<br><a target=\"_blank\" href=\"https://elog.psi.ch/elog/config.html\">Syntax Help</a>");

   rsprintf("</td></tr>\n");

   /*---- menu buttons ----*/

   rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");

   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Save"));
   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Cancel"));
   rsprintf("</span></td></tr>\n\n");

   rsprintf("</table>\n\n");
   show_bottom_text(lbs);
   rsprintf("</form></body></html>\r\n");
}