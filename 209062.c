void show_select_navigation(LOGBOOK *lbs) {
   int i, n_log;
   char str[NAME_LENGTH];
   char lbk_list[MAX_N_LIST][NAME_LENGTH];

   rsprintf("<tr><td class=\"menuframe\"><span class=\"menu4\">\n");

   rsprintf("<script language=\"JavaScript\" type=\"text/javascript\">\n");
   rsprintf("<!--\n");
   rsprintf("function ToggleAll()\n");
   rsprintf("  {\n");
   rsprintf("  for (var i = 0; i < document.form1.elements.length; i++)\n");
   rsprintf("    {\n");
   rsprintf
           ("    if (document.form1.elements[i].type == 'checkbox' && document.form1.elements[i].disabled == false)\n");
   rsprintf("      document.form1.elements[i].checked = !(document.form1.elements[i].checked);\n");
   rsprintf("    }\n");
   rsprintf("  }\n");
   rsprintf("//-->\n");
   rsprintf("</script>\n");

   rsprintf("%s:&nbsp;\n", loc("Selected entries"));

   rsprintf("<input type=button value=\"%s\" onClick=\"ToggleAll();\">\n", loc("Toggle all"));

   if (!getcfg(lbs->name, "Menu commands", str, sizeof(str)) || stristr(str, "Delete")) {
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Delete"));
   }

   if (!getcfg(lbs->name, "Menu commands", str, sizeof(str)) || stristr(str, "Edit")) {
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Edit"));
   }

   if (getcfg(lbs->name, "Menu commands", str, sizeof(str)) && stristr(str, "Copy to")) {
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Copy to"));
      rsprintf("<select name=destc>\n");

      if (getcfg(lbs->name, "Copy to", str, sizeof(str))) {
         n_log = strbreak(str, lbk_list, MAX_N_LIST, ",", FALSE);

         for (i = 0; i < n_log; i++)
            rsprintf("<option value=\"%s\">%s\n", lbk_list[i], lbk_list[i]);
      } else {
         for (i = 0;; i++) {
            if (!enumgrp(i, str))
               break;

            if (!is_logbook(str))
               continue;

            if (strieq(str, lbs->name))
               continue;

            rsprintf("<option value=\"%s\">%s\n", str, str);
         }
      }
      rsprintf("</select>\n");
   }

   if (getcfg(lbs->name, "Menu commands", str, sizeof(str)) && stristr(str, "Move to")) {
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Move to"));
      rsprintf("<select name=destm>\n");

      if (getcfg(lbs->name, "Move to", str, sizeof(str))) {
         n_log = strbreak(str, lbk_list, MAX_N_LIST, ",", FALSE);

         for (i = 0; i < n_log; i++)
            rsprintf("<option value=\"%s\">%s\n", lbk_list[i], lbk_list[i]);
      } else {
         for (i = 0;; i++) {
            if (!enumgrp(i, str))
               break;

            if (!is_logbook(str))
               continue;

            if (strieq(str, lbs->name))
               continue;

            rsprintf("<option value=\"%s\">%s\n", str, str);
         }
      }
      rsprintf("</select>\n");
   }

   rsprintf("</span></td></tr>\n\n");
}