void show_import_page_xml(LOGBOOK *lbs) {
   /*---- header ----*/

   show_html_header(lbs, FALSE, loc("ELOG XML import"), TRUE, FALSE, NULL, FALSE, 0);

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

   rsprintf("<tr><td class=\"attribname\" nowrap width=\"10%%\">%s:</td>\n", loc("Options"));
   rsprintf("<td class=\"attribvalue\">");

   if (isparam("head"))
      rsprintf("<input type=checkbox checked id=\"head\" name=\"head\" value=\"1\">\n");
   else
      rsprintf("<input type=checkbox id=\"head\" name=\"head\" value=\"1\">\n");
   rsprintf("<label for=\"head\">%s</label><br>\n", loc("Derive attributes from XML file"));

   if (isparam("keep"))
      rsprintf("<input type=checkbox checked id=\"keep\" name=\"keep\" value=\"1\">\n");
   else
      rsprintf("<input type=checkbox id=\"keep\" name=\"keep\" value=\"1\">\n");
   rsprintf("<label for=\"keep\">%s</label><br>\n",
            loc
                    ("Keep original entry IDs (may overwrite existing entries, but is required if imported entries contain replies)"));

   rsprintf("<input type=checkbox id=\"preview\" name=\"preview\" value=\"1\">\n");
   rsprintf("<label for=\"preview\">%s</label><br>\n", loc("Preview import"));

   rsprintf("</td></tr>\n");

   rsprintf("<tr><td class=\"attribname\" nowrap width=\"10%%\">%s:</td>\n", loc("XML filename"));
   rsprintf("<td class=\"attribvalue\">");

   if (isparam("xmlfile"))
      rsprintf("<b>%s</b>:<br>\n", loc("Please re-enter filename"));

   rsprintf("<input type=\"file\" size=\"60\" maxlength=\"200\" name=\"xmlfile\" ");
   if (isparam("xmlfile"))
      rsprintf(" value=\"%s\" ", getparam("xmlfile"));
   rsprintf("></td></tr>\n");

   rsprintf("</table></td></tr></table>\n\n");
   show_bottom_text(lbs);
   rsprintf("</form></body></html>\r\n");

}