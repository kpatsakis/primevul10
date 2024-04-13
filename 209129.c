void show_find_form(LOGBOOK *lbs) {
   int i, j, year, month, day, flag;
   char str[NAME_LENGTH], mode[NAME_LENGTH], comment[NAME_LENGTH], option[NAME_LENGTH], login_name[256],
           full_name[256], user_email[256], enc_attr[NAME_LENGTH], whole_attr[NAME_LENGTH],
           attrib[MAX_N_ATTR][NAME_LENGTH];

   /*---- header ----*/

   show_standard_header(lbs, FALSE, loc("ELOG find"), NULL, FALSE, NULL, NULL, 0);

   /*---- title ----*/

   show_standard_title(lbs, "", 0);

   /*---- menu buttons ----*/

   rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");

   rsprintf("<input type=submit value=\"%s\">\n", loc("Search"));
   rsprintf("<input type=reset value=\"%s\">\n", loc("Reset Form"));
   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Back"));
   rsprintf("<input type=hidden name=jcmd value=\"\">\n");
   rsprintf("</span></td></tr>\n\n");

   /*---- evaluate conditional attributes ----*/

   for (i = 0; i < lbs->n_attr; i++)
      attrib[i][0] = 0;

   /* get attributes from parameters */
   attrib_from_param(lbs->n_attr, attrib);

   evaluate_conditions(lbs, attrib);

   /*---- entry form ----*/

   rsprintf("<tr><td class=\"form1\"><table><tr><td valign=\"top\" class=\"form1\">\n");

   rsprintf("<b>%s:</b><br>", loc("Mode"));

   if (!getcfg(lbs->name, "Display mode", mode, sizeof(mode)))
      strcpy(mode, "Full");

   if (!getcfg(lbs->name, "Show text", str, sizeof(str)) || atoi(str) == 1) {
      if (strieq(mode, "Full"))
         rsprintf("<input type=radio id=\"full\" name=\"mode\" value=\"full\" checked>");
      else
         rsprintf("<input type=radio id=\"full\" name=\"mode\" value=\"full\">");
      rsprintf("<label for=\"full\">%s&nbsp;&nbsp;</label><br>\n", loc("Display full entries"));

      if (strieq(mode, "Summary"))
         rsprintf("<input type=radio id=\"summary\" name=\"mode\" value=\"summary\" checked>");
      else
         rsprintf("<input type=radio id=\"summary\" name=\"mode\" value=\"summary\">");
      rsprintf("<label for=\"summary\">%s&nbsp;&nbsp;</label><br>\n", loc("Summary only"));

   } else {
      if (strieq(mode, "Full") || strieq(mode, "Summary"))
         rsprintf("<input type=radio id=\"summary\" name=\"mode\" value=\"summary\" checked>");
      else
         rsprintf("<input type=radio id=\"summary\" name=\"mode\" value=\"summary\">");
      rsprintf("<label for=\"summary\">%s&nbsp;&nbsp;</label><br>\n", loc("Summary"));
   }

   if (strieq(mode, "Threaded"))
      rsprintf("<input type=radio id=\"threaded\" name=\"mode\" value=\"threaded\" checked>");
   else
      rsprintf("<input type=radio id=\"threaded\" name=\"mode\" value=\"threaded\">");
   rsprintf("<label for=\"threaded\">%s&nbsp;&nbsp;</label>\n", loc("Display threads"));

   rsprintf("</td><td valign=\"top\" class=\"form1\"><b>%s:</b><br>", loc("Export to"));

   if (strieq(mode, "CSV1"))
      rsprintf("<input type=radio id=\"CSV1\" name=\"mode\" value=\"CSV1\" checked>");
   else
      rsprintf("<input type=radio id=\"CSV1\" name=\"mode\" value=\"CSV1\">");
   rsprintf("<label for=\"CSV1\">%s&nbsp;&nbsp;</label><br>\n", loc("CSV (\",\" separated)"));

   if (strieq(mode, "CSV2"))
      rsprintf("<input type=radio id=\"CSV2\" name=\"mode\" value=\"CSV2\" checked>");
   else
      rsprintf("<input type=radio id=\"CSV2\" name=\"mode\" value=\"CSV2\">");
   rsprintf("<label for=\"CSV2\">%s&nbsp;&nbsp;</label><br>\n", loc("CSV (\";\" separated)"));

   if (strieq(mode, "CSV3"))
      rsprintf("<input type=radio id=\"CSV3\" name=\"mode\" value=\"CSV3\" checked>");
   else
      rsprintf("<input type=radio id=\"CSV3\" name=\"mode\" value=\"CSV3\">");
   rsprintf("<label for=\"CSV3\">%s&nbsp;&nbsp;</label><br>\n", loc("CSV (\";\" separated) + Text"));

   if (strieq(mode, "XML"))
      rsprintf("<input type=radio id=\"XML\" name=\"mode\" value=\"XML\" checked>");
   else
      rsprintf("<input type=radio id=\"XML\" name=\"mode\" value=\"XML\">");
   rsprintf("<label for=\"XML\">XML&nbsp;&nbsp;</label><br>\n");

   if (strieq(mode, "Raw"))
      rsprintf("<input type=radio id=\"Raw\" name=\"mode\" value=\"Raw\" checked>");
   else
      rsprintf("<input type=radio id=\"Raw\" name=\"mode\" value=\"Raw\">");
   rsprintf("<label for=\"Raw\">Raw&nbsp;&nbsp;</label><br>\n");

   rsprintf("</td>\n");

   rsprintf("<td valign=\"top\" class=\"form2\"><b>%s:</b><br>", loc("Options"));

   rsprintf("<input type=checkbox id=\"attach\" name=\"attach\" value=1>");
   rsprintf("<label for=\"attach\">%s<br></label>\n", loc("Show attachments"));

   rsprintf("<input type=checkbox id=\"printable\" name=\"printable\" value=1>");
   rsprintf("<label for=\"printable\">%s<br></label>\n", loc("Printable output"));

   /* put hidden reverse=0, which gets used if the reverse checkbox is unchecked and "reverse sort=1" is in elogd.cfg */
   rsprintf("<input type=hidden name=\"reverse\" value=0>\n");
   if (getcfg(lbs->name, "Reverse sort", str, sizeof(str)) && atoi(str) == 1)
      rsprintf("<input type=checkbox id=\"reverse\" name=\"reverse\" value=1 checked>");
   else
      rsprintf("<input type=checkbox id=\"reverse\" name=\"reverse\" value=1>");
   rsprintf("<label for=\"reverse\">%s<br></label>\n", loc("Sort in reverse order"));

   /* count logbooks */
   for (i = 0;; i++) {
      if (!enumgrp(i, str))
         break;

      if (is_logbook(str))
         continue;
   }

   if (i > 2) {
      if (!getcfg(lbs->name, "Search all logbooks", str, sizeof(str)) || atoi(str) == 1 || atoi(str) == 2) {

         if (atoi(str) == 2)
            rsprintf("<input type=checkbox checked id=all name=all value=1>");
         else
            rsprintf("<input type=checkbox id=all name=all value=1>");
         rsprintf("<label for=\"all\">%s</label><br>\n", loc("Search all logbooks"));
      }
   }

   rsprintf(loc("Display"));
   if (!getcfg(lbs->name, "Entries per page", str, sizeof(str)))
      strcpy(str, "20");
   rsprintf(" <input type=text name=npp size=3 value=%s> ", str);
   rsprintf(loc("entries per page"));

   rsprintf("</td></tr></table></td></tr>\n");

   rsprintf("<tr><td class=\"form2\"><b>%s:</b>", loc("Filters"));
   sprintf(str, "<a href=\"http://dmoz.org/Computers/Programming/Languages/Regular_Expressions/\">");
   strcat(str, loc("regular expressions"));
   strcat(str, "</a>");
   rsprintf("&nbsp;&nbsp;<span class=\"selcomment\">(");
   rsprintf(loc("Text fields are treated as %s"), str);
   rsprintf(")</span><br>");

   /* table for two-column items */
   rsprintf("<table width=\"100%%\" cellspacing=0>\n");

   rsprintf("<tr><td class=\"attribname\" nowrap width=\"10%%\">%s:</td>", loc("Entry date"));
   rsprintf("<td class=\"attribvalue\"><table width=\"100%%\" cellspacing=0 border=0>\n");
   rsprintf("<tr><td width=\"1%%\">%s:<td>", loc("Start"));

   year = month = day = 0;
   sprintf(str, "ya");
   if (isparam(str))
      year = atoi(getparam(str));
   sprintf(str, "ma");
   if (isparam(str))
      month = atoi(getparam(str));
   sprintf(str, "da");
   if (isparam(str))
      day = atoi(getparam(str));
   show_date_selector(day, month, year, "a");
   rsprintf("&nbsp;&nbsp;");
   show_time_selector(-1, -1, -1, "a");

   rsprintf("&nbsp;&nbsp;/&nbsp;&nbsp;%s:&nbsp;", loc("Show last"));

   if (getcfg(lbs->name, "Show last default", str, sizeof(str)))
      i = atoi(str);
   else
      i = 0;

   rsprintf("<select name=last>\n");
   if (i > 0)
      rsprintf("<option value=\"\">%s\n", loc("All time"));
   else
      rsprintf("<option value=\"\">\n");
   rsprintf("<option %svalue=1>%s\n", i == 1 ? "selected " : "", loc("Day"));
   rsprintf("<option %svalue=3>%s\n", i == 3 ? "selected " : "", loc("3 Days"));
   rsprintf("<option %svalue=7>%s\n", i == 7 ? "selected " : "", loc("Week"));
   rsprintf("<option %svalue=31>%s\n", i == 31 ? "selected " : "", loc("Month"));
   rsprintf("<option %svalue=92>%s\n", i == 92 ? "selected " : "", loc("3 Months"));
   rsprintf("<option %svalue=182>%s\n", i == 182 ? "selected " : "", loc("6 Months"));
   rsprintf("<option %svalue=364>%s\n", i == 364 ? "selected " : "", loc("Year"));
   rsprintf("</select> \n");

   rsprintf("</td></tr>\n");

   rsprintf("<tr><td width=\"1%%\">%s:<td>", loc("End"));

   year = month = day = 0;
   sprintf(str, "yb");
   if (isparam(str))
      year = atoi(getparam(str));
   sprintf(str, "mb");
   if (isparam(str))
      month = atoi(getparam(str));
   sprintf(str, "db");
   if (isparam(str))
      day = atoi(getparam(str));
   show_date_selector(day, month, year, "b");
   rsprintf("&nbsp;&nbsp;");
   show_time_selector(-1, -1, -1, "b");

   rsprintf("</td></tr></table></td></tr>\n");

   for (i = 0; i < lbs->n_attr; i++) {
      rsprintf("<tr><td class=\"attribname\" nowrap>%s:</td>", attr_list[i]);
      rsprintf("<td class=\"attribvalue\">");
      if (attr_options[i][0][0] == 0) {

         if (attr_flags[i] & (AF_DATE | AF_DATETIME)) {

            rsprintf("<table width=\"100%%\" cellspacing=0 border=0>\n");
            rsprintf("<tr><td width=\"1%%\">%s:<td>", loc("Start"));

            year = month = day = 0;
            sprintf(str, "y%da", i);
            if (isparam(str))
               year = atoi(getparam(str));
            sprintf(str, "m%da", i);
            if (isparam(str))
               month = atoi(getparam(str));
            sprintf(str, "d%da", i);
            if (isparam(str))
               day = atoi(getparam(str));

            sprintf(str, "%da", i);
            show_date_selector(day, month, year, str);
            if (attr_flags[i] & AF_DATETIME) {
               rsprintf("&nbsp;&nbsp;");
               show_time_selector(-1, -1, -1, str);
            }

            rsprintf("</td></tr>\n");
            rsprintf("<tr><td width=\"1%%\">%s:<td>", loc("End"));

            year = month = day = 0;
            sprintf(str, "y%db", i);
            if (isparam(str))
               year = atoi(getparam(str));
            sprintf(str, "m%db", i);
            if (isparam(str))
               month = atoi(getparam(str));
            sprintf(str, "d%db", i);
            if (isparam(str))
               day = atoi(getparam(str));

            sprintf(str, "%db", i);
            show_date_selector(day, month, year, str);
            if (attr_flags[i] & AF_DATETIME) {
               rsprintf("&nbsp;&nbsp;");
               show_time_selector(-1, -1, -1, str);
            }

            rsprintf("</td></tr></table>\n");

         } else if (attr_flags[i] & AF_MUSERLIST) {

            for (j = 0;; j++) {
               if (!enum_user_line(lbs, j, login_name, sizeof(login_name)))
                  break;
               get_user_line(lbs, login_name, NULL, full_name, NULL, NULL, NULL, NULL);

               sprintf(str, "%s_%d", attr_list[i], j);

               rsprintf("<nobr><input type=checkbox id=\"%s\" name=\"%s\" value=\"%s\"\">\n", str, str,
                        full_name);

               rsprintf("<label for=\"%s\">%s</label></nobr>\n", str, full_name);
            }

         } else if (attr_flags[i] & AF_MUSEREMAIL) {

            for (j = 0;; j++) {
               if (!enum_user_line(lbs, j, login_name, sizeof(login_name)))
                  break;
               get_user_line(lbs, login_name, NULL, NULL, user_email, NULL, NULL, NULL);

               sprintf(str, "%s_%d", attr_list[i], j);

               rsprintf("<nobr><input type=checkbox id=\"%s\" name=\"%s\" value=\"%s\"\">\n", str, str,
                        user_email);

               rsprintf("<label for=\"%s\">%s</label></nobr>\n", str, user_email);
            }

         } else {
            rsprintf("<input type=\"text\" size=\"30\" maxlength=\"80\" name=\"%s\" value=\"%s\">\n",
                     attr_list[i], attrib[i]);
         }

      } else {
         if (strieq(attr_options[i][0], "boolean")) {

            if (isparam(attr_list[i]) && *getparam(attr_list[i]))
               flag = atoi(getparam(attr_list[i]));
            else
               flag = -1;

            sprintf(str, "%s_0", attr_list[i]);
            rsprintf("<span style=\"white-space:nowrap;\">\n");
            if (flag == 0)
               rsprintf("<input type=radio checked id=\"%s\" name=\"%s\" value=\"0\">\n", str, attr_list[i]);
            else
               rsprintf("<input type=radio id=\"%s\" name=\"%s\" value=\"0\">\n", str, attr_list[i]);
            rsprintf("<label for=\"%s\">0</label>\n", str);
            rsprintf("</span>\n");

            sprintf(str, "%s_1", attr_list[i]);
            rsprintf("<span style=\"white-space:nowrap;\">\n");
            if (flag == 1)
               rsprintf("<input type=radio checked id=\"%s\" name=\"%s\" value=\"1\">\n", str, attr_list[i]);
            else
               rsprintf("<input type=radio id=\"%s\" name=\"%s\" value=\"1\">\n", str, attr_list[i]);
            rsprintf("<label for=\"%s\">1</label>\n", str);
            rsprintf("</span>\n");

            sprintf(str, "%s_2", attr_list[i]);
            rsprintf("<span style=\"white-space:nowrap;\">\n");
            if (flag == -1)
               rsprintf("<input type=radio checked id=\"%s\" name=\"%s\" value=\"\">\n", str, attr_list[i]);
            else
               rsprintf("<input type=radio id=\"%s\" name=\"%s\" value=\"\">\n", str, attr_list[i]);
            rsprintf("<label for=\"%s\">%s</label>\n", str, loc("unspecified"));
            rsprintf("</span>\n");
         }

            /* display image for icon */
         else if (attr_flags[i] & AF_ICON) {
            for (j = 0; j < MAX_N_LIST && attr_options[i][j][0]; j++) {
               strcpy(option, attr_options[i][j]);
               if (strchr(option, '{'))
                  *strchr(option, '{') = 0;

               sprintf(str, "Icon comment %s", option);
               getcfg(lbs->name, str, comment, sizeof(comment));

               if (comment[0] == 0)
                  strcpy(comment, option);

               rsprintf("<nobr><input type=radio name=\"%s\" value=\"%s\">", attr_list[i], comment);

               rsprintf("<img src=\"icons/%s\" alt=\"%s\" title=\"%s\"></nobr>\n", option, comment, comment);
            }
         }

            /* display check boxes (or'ed) */
         else if (attr_flags[i] & AF_MULTI) {
            for (j = 0; j < MAX_N_LIST && attr_options[i][j][0]; j++) {
               sprintf(str, "%s_%d", attr_list[i], j);

               if (isparam(str))
                  rsprintf("<nobr><input type=checkbox checked id=\"%s\" name=\"%s\" value=\"%s\"\">\n",
                           str, str, attr_options[i][j]);
               else
                  rsprintf("<nobr><input type=checkbox id=\"%s\" name=\"%s\" value=\"%s\"\">\n",
                           str, str, attr_options[i][j]);

               rsprintf("<label for=\"%s\">%s</label></nobr>\n", str, attr_options[i][j]);
            }
         } else {
            rsprintf("<select name=\"%s\"", attr_list[i]);

            if (is_cond_attr(i))
               rsprintf(" onChange=\"document.form1.jcmd.value='Find';document.form1.submit();\"");
            rsprintf(">\n");

            rsprintf("<option value=\"\">\n");
            for (j = 0; j < MAX_N_LIST && attr_options[i][j][0]; j++) {
               strencode2(str, attr_options[i][j], sizeof(str));
               if (strchr(str, '{'))
                  *strchr(str, '{') = 0;
               strencode2(enc_attr, attrib[i], sizeof(enc_attr));

               sprintf(whole_attr, "^%s$", str);
               if (strieq(attr_options[i][j], attrib[i]) || strieq(str, enc_attr)
                   || strieq(whole_attr, enc_attr))
                  rsprintf("<option selected value=\"^%s$\">%s\n", str, str);
               else
                  rsprintf("<option value=\"^%s$\">%s\n", str, str);
            }
            rsprintf("</select>\n");
         }
      }
      rsprintf("</td></tr>\n");
   }

   rsprintf("<tr><td class=\"attribname\">%s:</td>", loc("Text"));
   rsprintf("<td class=\"attribvalue\">\n");
   if (isparam("subtext"))
      strlcpy(str, getparam("subtext"), sizeof(str));
   else
      str[0] = 0;
   rsprintf("<input type=\"text\" size=\"30\" maxlength=\"80\" name=\"subtext\" value=\"%s\">\n", str);

   rsprintf("<input type=checkbox id=\"sall\" name=\"sall\" value=1>\n");
   rsprintf("<label for=\"sall\">%s</label>\n", loc("Search text also in attributes"));

   rsprintf("<tr><td><td class=\"attribvalue\">\n");
   if (getcfg(lbs->name, "Case sensitive search", str, sizeof(str)) && atoi(str))
      rsprintf("<input type=checkbox id=\"sall\" name=\"casesensitive\" value=1 checked>\n");
   else
      rsprintf("<input type=checkbox id=\"sall\" name=\"casesensitive\" value=1>\n");
   rsprintf("<label for=\"casesensitive\">%s</label>\n", loc("Case sensitive"));

   rsprintf("</td></tr></table></td></tr></table>\n");
   show_bottom_text(lbs);
   rsprintf("</form></body></html>\r\n");
}