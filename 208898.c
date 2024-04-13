void show_import_page_csv(LOGBOOK *lbs) {
   char str[256], str2[256];

   /*---- header ----*/

   show_html_header(lbs, FALSE, loc("ELOG CSV import"), TRUE, FALSE, NULL, FALSE, 0);

   rsprintf("<body><form method=\"POST\" action=\"./\" enctype=\"multipart/form-data\">\n");

   /*---- title ----*/

   show_standard_title(lbs, "", 0);

   /*---- menu buttons ----*/

   rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");

   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Cancel"));
   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Import"));

   rsprintf("</span></td></tr>\n\n");

   /* table for two-column items */
   rsprintf("<tr><td class=\"form2\">");
   rsprintf("<table width=\"100%%\" cellspacing=0>\n");

   /*---- entry form ----*/

   rsprintf("<tr><td class=\"attribname\" nowrap width=\"10%%\">%s:</td>\n", loc("Field separator"));
   rsprintf("<td class=\"attribvalue\">");

   str[0] = 0;
   if (isparam("sep"))
      strlcpy(str, getparam("sep"), sizeof(str));

   if (str[0] == 0)
      rsprintf("<input type=\"radio\" checked id=\"comma\" name=\"sep\" value=\"auto\">");
   else
      rsprintf("<input type=\"radio\" id=\"comma\" name=\"sep\" value=\"auto\">");
   rsprintf("<label for=\"comma\">%s</label>\n", loc("Auto detect"));

   if (str[0] == ',')
      rsprintf("<input type=\"radio\" checked id=\"comma\" name=\"sep\" value=\",\">");
   else
      rsprintf("<input type=\"radio\" id=\"comma\" name=\"sep\" value=\",\">");
   rsprintf("<label for=\"comma\">%s (,)</label>\n", loc("Comma"));

   if (str[0] == ';')
      rsprintf("<input type=\"radio\" checked id=\"semi\" name=\"sep\" value=\";\">");
   else
      rsprintf("<input type=\"radio\" id=\"semi\" name=\"sep\" value=\";\">");
   rsprintf("<label for=\"semi\">%s (;)</label>\n", loc("Semicolon"));

   rsprintf("</td></tr>\n");

   rsprintf("<tr><td class=\"attribname\" nowrap width=\"10%%\">%s:</td>\n", loc("Options"));
   rsprintf("<td class=\"attribvalue\">");

   if (isparam("head"))
      rsprintf("<input type=checkbox checked id=\"head\" name=\"head\" value=\"1\">\n");
   else
      rsprintf("<input type=checkbox id=\"head\" name=\"head\" value=\"1\">\n");
   rsprintf("<label for=\"head\">%s</label><br>\n", loc("Derive attributes from CSV file"));

   if (isparam("notignore"))
      rsprintf("<input type=checkbox checked id=\"notignore\" name=\"notignore\" value=\"1\">\n");
   else
      rsprintf("<input type=checkbox id=\"notignore\" name=\"notignore\" value=\"1\">\n");
   rsprintf("<label for=\"notignore\">%s</label><br>\n", loc("Do not ignore first line"));

   rsprintf("<input type=checkbox id=\"preview\" name=\"preview\" value=\"1\">\n");
   rsprintf("<label for=\"preview\">%s</label><br>\n", loc("Preview import"));

   if (isparam("filltext"))
      rsprintf("<input type=checkbox checked id=\"filltext\" name=\"filltext\" value=\"1\">\n");
   else
      rsprintf("<input type=checkbox id=\"filltext\" name=\"filltext\" value=\"1\">\n");
   strcpy(str, loc("text"));
   sprintf(str2, loc("Column header '%s' must be present in CSV file"), str);
   rsprintf("<label for=\"filltext\">%s (%s)</label><br>\n", loc("Fill text body"), str2);

   rsprintf("</td></tr>\n");

   rsprintf("<tr><td class=\"attribname\" nowrap width=\"10%%\">%s:</td>\n", loc("CSV filename"));

   rsprintf("<td class=\"attribvalue\">");

   if (isparam("csvfile"))
      rsprintf("<b>%s</b>:<br>\n", loc("Please re-enter filename"));

   rsprintf("<input type=\"file\" size=\"60\" maxlength=\"200\" name=\"csvfile\" ");
   if (isparam("csvfile"))
      rsprintf(" value=\"%s\" ", getparam("csvfile"));
   rsprintf("></td></tr>\n");

   rsprintf("</table></td></tr></table>\n\n");
   show_bottom_text(lbs);
   rsprintf("</form></body></html>\r\n");
}