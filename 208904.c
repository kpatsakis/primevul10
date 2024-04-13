void show_upgrade_page(LOGBOOK *lbs) {
   char str[1000];

   show_html_header(lbs, FALSE, "ELOG Upgrade Information", TRUE, FALSE, NULL, FALSE, 0);

   rsprintf("<body>\n");

   rsprintf("<table class=\"frame\" cellpadding=\"0\" cellspacing=\"0\">\n\n");

   rsprintf("<tr><td class=\"title2\">ELog Electronic Logbook Upgrade Information</font></td></tr>\n");

   rsprintf("<tr><td class=\"form1\"><br>\n");

   rsprintf("You probably use an <b>%s</b> configuration file for a ELOG version\n", CFGFILE);
   rsprintf("1.1.x, since it contains a <b><code>\"Types = ...\"</code></b> entry. From version\n");
   rsprintf("1.2.0 on, the fixed attributes <b>Type</b> and <b>Category</b> have been\n");
   rsprintf("replaced by arbitrary attributes. Please replace these two lines with the\n");
   rsprintf("following entries:<p>\n");
   rsprintf("<pre>\n");
   rsprintf("Attributes = Author, Type, Category, Subject\n");
   rsprintf("Required Attributes = Author\n");
   getcfg(lbs->name, "Types", str, sizeof(str));
   rsprintf("Options Type = %s\n", str);
   getcfg(lbs->name, "Categories", str, sizeof(str));
   rsprintf("Options Category = %s\n", str);
   rsprintf("Page title = $subject\n");
   rsprintf("</pre>\n");
   rsprintf("<p>\n");

   rsprintf("It is of course possible to change the attributes or add new ones. The new\n");
   rsprintf("options in the configuration file are described under <a href=\"\n");
   rsprintf("https://elog.psi.ch/elog/config.html\">https://elog.psi.ch/elog/config.html\n");
   rsprintf("</a>.\n");

   rsprintf("</td></tr></table>\n\n");

   rsprintf("<hr>\n");
   rsprintf("<address>\n");
   rsprintf("<a href=\"https://www.psi.ch/ltp-muon-physics/stefan-ritt\">S. Ritt</a>, 18 October 2001");
   rsprintf("</address>");
   show_bottom_text(lbs);
   rsprintf("</body></html>\r\n");
}