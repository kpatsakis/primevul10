void show_new_user_page(LOGBOOK *lbs, char *user) {
   char str[256];

   /*---- header ----*/

   show_html_header(lbs, TRUE, loc("ELOG new user"), TRUE, FALSE, NULL, FALSE, 0);
   rsprintf("<body><center><br><br>\n");
   show_top_text(lbs);
   rsprintf("<form name=\"form1\" id=\"form1\" method=\"GET\" action=\".\">\n\n");

   /*---- title ----*/

   if (lbs)
      show_standard_title(lbs, "", 1);
   else
      show_standard_title(NULL, "", 1);

   /* table for two-column items */
   rsprintf("<tr><td class=\"form2\">");
   rsprintf("<table width=\"100%%\" cellspacing=0>\n");

   /*---- entry form ----*/

   rsprintf("<tr><td nowrap>%s:</td>\n", loc("Login name"));
   if (user && user[0]) {
      strencode2(str, user, sizeof(str));
      rsprintf("<td><input type=text size=40 name=new_user_name value=\"%s\" readonly></td>\n", str);
      rsprintf("<td>&nbsp;</td>\n");
   } else {
      rsprintf("<td><input type=text size=40 name=new_user_name></td>\n");
      rsprintf("<td nowrap align=left><font size=2><i>(%s)</i></font></td></tr>\n",
               loc("name may not contain blanks"));
   }

   rsprintf("<tr><td nowrap>%s:</td>\n", loc("Full name"));
   rsprintf("<td colspan=2><input type=text size=40 name=new_full_name></tr>\n");

   rsprintf("<tr><td nowrap>Email:</td>\n");
   rsprintf("<td colspan=2><input type=text size=40 name=new_user_email></tr>\n");
   getcfg(lbs->name, "Authentication", str, sizeof(str));
   if (!stristr(str, "Kerberos") && !stristr(str, "Webserver") && !stristr(str, "PAM")) {
      rsprintf("<tr><td nowrap>%s:</td>\n", loc("Password"));
      rsprintf("<td colspan=2><input type=password size=40 name=newpwd>\n");

      rsprintf("<tr><td nowrap>%s:</td>\n", loc("Retype password"));
      rsprintf("<td colspan=2><input type=password size=40 name=newpwd2>\n");
   }
   rsprintf("</td></tr></table>\n");

   /*---- menu buttons ----*/

   rsprintf("<tr><td class=\"menucenter\">\n");
   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Save"));
   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Cancel"));
   rsprintf("</td></tr>\n\n");

   rsprintf("</td></tr></table>\n\n");
   rsprintf("</form></center></body></html>\r\n");
}