void show_page_filters(LOGBOOK *lbs, int n_msg, int page_n, BOOL mode_commands, char *mode) {
   int cur_exp, n, i, j, i1, i2, index, attr_index, size;
   char ref[256], str[NAME_LENGTH], comment[NAME_LENGTH], list[MAX_N_LIST][NAME_LENGTH],
           option[NAME_LENGTH], option_whole[NAME_LENGTH];

   rsprintf("<tr><td class=\"menuframe\">\n");
   rsprintf("<table width=\"100%%\" border=0 cellpadding=\"0\" cellspacing=\"0\">\n");

   rsprintf("<tr>\n");

   if (mode_commands) {
      rsprintf("<td class=\"menu2a\">\n");

      if (!getcfg(lbs->name, "Show text", str, sizeof(str)) || atoi(str) == 1) {
         if (page_n != 1)
            sprintf(ref, "page%d", page_n);
         else
            ref[0] = 0;
         build_ref(ref, sizeof(ref), "full", "", "", "");

         if (strieq(mode, "full"))
            rsprintf("&nbsp;%s&nbsp;|", loc("Full"));
         else
            rsprintf("&nbsp;<a href=\"%s\">%s</a>&nbsp;|", ref, loc("Full"));
      }

      if (page_n != 1)
         sprintf(ref, "page%d", page_n);
      else
         ref[0] = 0;
      build_ref(ref, sizeof(ref), "summary", "", "", "");

      if (strieq(mode, "summary"))
         rsprintf("&nbsp;%s&nbsp;|", loc("Summary"));
      else
         rsprintf("&nbsp;<a href=\"%s\">%s</a>&nbsp;|", ref, loc("Summary"));

      if (page_n != 1)
         sprintf(ref, "page%d", page_n);
      else
         ref[0] = 0;
      build_ref(ref, sizeof(ref), "threaded", "", "", "");

      if (strieq(mode, "threaded"))
         rsprintf("&nbsp;%s&nbsp;", loc("Threaded"));
      else
         rsprintf("&nbsp;<a href=\"%s\">%s</a>&nbsp;", ref, loc("Threaded"));

      if (strieq(mode, "full")) {
         if (page_n != 1)
            sprintf(ref, "page%d", page_n);
         else
            ref[0] = 0;

         cur_exp = 0;
         if (strieq(mode, "full"))
            cur_exp = 1;
         if (isparam("elattach"))
            cur_exp = atoi(getparam("elattach"));
         if (isparam("attach"))
            cur_exp = atoi(getparam("attach"));

         if (cur_exp) {
            build_ref(ref, sizeof(ref), "", "", "0", "");
            rsprintf("|&nbsp;<a href=\"%s\">%s</a>&nbsp;", ref, loc("Hide attachments"));
         } else {
            build_ref(ref, sizeof(ref), "", "", "1", "");
            rsprintf("|&nbsp;<a href=\"%s\">%s</a>&nbsp;", ref, loc("Show attachments"));
         }
      }

      if (strieq(mode, "threaded")) {
         if (page_n != 1)
            sprintf(ref, "page%d", page_n);
         else
            ref[0] = 0;

         cur_exp = 1;
         if (getcfg(lbs->name, "Expand default", str, sizeof(str)))
            cur_exp = atoi(str);
         if (isparam("expand"))
            cur_exp = atoi(getparam("expand"));

         if (cur_exp > 0) {
            sprintf(str, "%d", cur_exp > 0 ? cur_exp - 1 : 0);
            build_ref(ref, sizeof(ref), "", str, "", "");
            rsprintf("|&nbsp;<a href=\"%s\">%s</a>&nbsp;", ref, loc("Collapse"));
         } else
            rsprintf("|&nbsp;%s&nbsp;", loc("Collapse"));

         if (cur_exp < 3) {
            if (page_n != 1)
               sprintf(ref, "page%d", page_n);
            else
               ref[0] = 0;
            sprintf(str, "%d", cur_exp < 3 ? cur_exp + 1 : 3);
            build_ref(ref, sizeof(ref), "", str, "", "");
            rsprintf("|&nbsp;<a href=\"%s\">%s</a>&nbsp;", ref, loc("Expand"));
         } else
            rsprintf("|&nbsp;%s&nbsp;", loc("Expand"));
      }

      rsprintf("</td>\n");
   }

   rsprintf("<td class=\"menu2b\">\n");

   /*---- filter menu text ----*/

   if (getcfg(lbs->name, "filter menu text", str, sizeof(str))) {
      FILE *f;
      char file_name[256], *buf;

      /* check if file starts with an absolute directory */
      if (str[0] == DIR_SEPARATOR || str[1] == ':')
         strcpy(file_name, str);
      else {
         strlcpy(file_name, logbook_dir, sizeof(file_name));
         strlcat(file_name, str, sizeof(file_name));
      }

      f = fopen(file_name, "rb");
      if (f != NULL) {
         fseek(f, 0, SEEK_END);
         size = TELL(fileno(f));
         fseek(f, 0, SEEK_SET);

         buf = (char *) xmalloc(size + 1);
         fread(buf, 1, size, f);
         buf[size] = 0;
         fclose(f);

         rsputs(buf);

      } else
         rsprintf("<center><b>Error: file <i>\"%s\"</i> not found</b></center>", file_name);
   }

   ref[0] = 0;
   if (!isparam("new_entries") || atoi(getparam("new_entries")) == 0) {
      build_ref(ref, sizeof(ref), "", "", "", "1");
      rsprintf
              ("<a href=\"%s\"><img align=\"middle\" border=\"0\" src=\"new_entry.png\" alt=\"%s\" title=\"%s\"></a>&nbsp;&nbsp;",
               ref, loc("Show only new entries"), loc("Show only new entries"));
   } else {
      build_ref(ref, sizeof(ref), "", "", "", "0");
      rsprintf
              ("<a href=\"%s\"><img align=\"middle\" border=\"0\" src=\"all_entry.png\" alt=\"%s\" title=\"%s\"></a>&nbsp;&nbsp;",
               ref, loc("Show all entries"), loc("Show all entries"));
   }

   if (getcfg(lbs->name, "Quick filter", str, sizeof(str))) {

      n = strbreak(str, list, MAX_N_LIST, ",", FALSE);

      if (getcfg(lbs->name, "Case sensitive search", str, sizeof(str)) && atoi(str))
         rsprintf("<input type=hidden name=\"casesensitive\" value=1>\n");

      for (index = 0; index < n; index++) {
         /* find according attribute index */
         for (attr_index = 0; attr_index < lbs->n_attr; attr_index++)
            if (strieq(list[index], attr_list[attr_index]))
               break;

         if (attr_index == lbs->n_attr && !strieq(list[index], "Date") && !strieq(list[index], "Subtext")
             && !strieq(list[index], "ID")) {
            rsprintf("Error: Attribute \"%s\" for quick filter not found", list[index]);
            attr_index = 0;
         }

         if (strieq(list[index], "Date")) {
            i = isparam("last") ? atoi(getparam("last")) : 0;

            if (i == 0 && getcfg(lbs->name, "Last default", str, sizeof(str))) {
               i = atoi(str);
               setparam("last", str);
            }

            rsprintf("<select title=\"%s\" name=last onChange=\"document.form1.submit()\">\n",
                     loc("Select period"));

            rsprintf("<option value=\"_all_\">-- %s --\n", loc("All entries"));

            rsprintf("<option %s value=1>%s\n", i == 1 ? "selected" : "", loc("Last day"));
            rsprintf("<option %s value=3>%s\n", i == 3 ? "selected" : "", loc("Last 3 Days"));
            rsprintf("<option %s value=7>%s\n", i == 7 ? "selected" : "", loc("Last week"));
            rsprintf("<option %s value=31>%s\n", i == 31 ? "selected" : "", loc("Last month"));
            rsprintf("<option %s value=92>%s\n", i == 92 ? "selected" : "", loc("Last 3 Months"));
            rsprintf("<option %s value=182>%s\n", i == 182 ? "selected" : "", loc("Last 6 Months"));
            rsprintf("<option %s value=364>%s\n", i == 364 ? "selected" : "", loc("Last Year"));

            rsprintf("</select>\n");
         } else if (strieq(attr_options[attr_index][0], "boolean")) {

            sprintf(str, loc("Select %s"), list[index]);
            rsprintf("<select title=\"%s\" name=\"%s\" onChange=\"document.form1.submit()\">\n", str,
                     list[index]);

            rsprintf("<option value=\"_all_\">-- %s --\n", list[index]);

            if (isparam(attr_list[attr_index]) && strieq("1", getparam(attr_list[attr_index])))
               rsprintf("<option selected value=\"1\">1\n");
            else
               rsprintf("<option value=\"1\">1\n");

            if (isparam(attr_list[attr_index]) && strieq("0", getparam(attr_list[attr_index])))
               rsprintf("<option selected value=\"0\">0\n");
            else
               rsprintf("<option value=\"0\">0\n");

            rsprintf("</select>\n");
         } else {

            /* check if attribute has options */
            if (attr_list[attr_index][0] == 0 || attr_options[attr_index][0][0] == 0) {

               if (attr_flags[attr_index] & (AF_DATE | AF_DATETIME)) {

                  rsprintf("<select name=\"%s\" onChange=\"document.form1.submit()\">\n", list[index]);

                  i = isparam(list[index]) ? atoi(getparam(list[index])) : 0;

                  rsprintf("<option %s value=-364>%s %s\n", i == -364 ? "selected" : "", loc("Last"),
                           loc("Year"));
                  rsprintf("<option %s value=-182>%s %s\n", i == -182 ? "selected" : "", loc("Last"),
                           loc("6 Months"));
                  rsprintf("<option %s value=-92>%s %s\n", i == -92 ? "selected" : "", loc("Last"),
                           loc("3 Months"));
                  rsprintf("<option %s value=-31>%s %s\n", i == -31 ? "selected" : "", loc("Last"),
                           loc("Month"));
                  rsprintf("<option %s value=-7>%s %s\n", i == -7 ? "selected" : "", loc("Last"),
                           loc("Week"));
                  rsprintf("<option %s value=-3>%s %s\n", i == -3 ? "selected" : "", loc("Last"),
                           loc("3 Days"));
                  rsprintf("<option %s value=-1>%s %s\n", i == -1 ? "selected" : "", loc("Last"), loc("Day"));

                  sprintf(str, "-- %s  --", list[index]);
                  rsprintf("<option %s value=\"_all_\">%s\n", i == 0 ? "selected" : "", str);

                  rsprintf("<option %s value=1>%s %s\n", i == 1 ? "selected" : "", loc("Next"), loc("Day"));
                  rsprintf("<option %s value=3>%s %s\n", i == 3 ? "selected" : "", loc("Next"),
                           loc("3 Days"));
                  rsprintf("<option %s value=7>%s %s\n", i == 7 ? "selected" : "", loc("Next"), loc("Week"));
                  rsprintf("<option %s value=31>%s %s\n", i == 31 ? "selected" : "", loc("Next"),
                           loc("Month"));
                  rsprintf("<option %s value=92>%s %s\n", i == 92 ? "selected" : "", loc("Next"),
                           loc("3 Months"));
                  rsprintf("<option %s value=182>%s %s\n", i == 182 ? "selected" : "", loc("Next"),
                           loc("6 Months"));
                  rsprintf("<option %s value=364>%s %s\n", i == 364 ? "selected" : "", loc("Next"),
                           loc("Year"));

                  rsprintf("</select>\n");
               } else {
                  if (strieq(list[index], "Subtext")) {
                     rsprintf
                             ("<input onClick=\"this.value='';\" title=\"%s\" type=text onChange=\"document.form1.submit()\"",
                              loc("Enter text"));
                     sprintf(str, "-- %s --", loc("Text"));
                     if (isparam(list[index]) && *getparam(list[index]))
                        strencode2(str, getparam(list[index]), sizeof(str));

                     rsprintf(" name=\"Subtext\" value=\"%s\">\n", str);
                  } else {
                     sprintf(str, loc("Enter %s"), list[index]);
                     rsprintf
                             ("<input onClick=\"this.value='';\" title=\"%s\" type=text onChange=\"document.form1.submit()\"",
                              str);
                     sprintf(str, "-- %s --", list[index]);
                     if (isparam(list[index]) && *getparam(list[index]))
                        strencode2(str, getparam(list[index]), sizeof(str));

                     rsprintf(" name=\"%s\" value=\"%s\">\n", list[index], str);
                  }
               }
            } else {

               sprintf(str, loc("Select %s"), list[index]);
               rsprintf("<select title=\"%s\" name=\"%s\" onChange=\"document.form1.submit()\">\n", str,
                        list[index]);

               rsprintf("<option value=\"_all_\">-- %s --\n", list[index]);

               for (j = 0; j < MAX_N_LIST && attr_options[attr_index][j][0]; j++) {
                  comment[0] = 0;
                  if (attr_flags[attr_index] & AF_ICON) {
                     sprintf(str, "Icon comment %s", attr_options[attr_index][j]);
                     getcfg(lbs->name, str, comment, sizeof(comment));
                  }

                  if (comment[0] == 0)
                     strcpy(comment, attr_options[attr_index][j]);

                  for (i1 = i2 = 0; i1 <= (int) strlen(comment); i1++) {
                     if (comment[i1] == '(') {
                        option[i2++] = '\\';
                        option[i2++] = '(';
                     } else if (comment[i1] == '{') {
                        option[i2] = 0;
                        break;
                     } else
                        option[i2++] = comment[i1];
                  }

                  sprintf(option_whole, "^%s$", option);
                  if (isparam(attr_list[attr_index]) &&
                      (strieq(option, getparam(attr_list[attr_index])) ||
                       strieq(option_whole, getparam(attr_list[attr_index]))))
                     rsprintf("<option selected value=\"%s\">%s\n", option_whole, option);
                  else
                     rsprintf("<option value=\"%s\">%s\n", option_whole, option);
               }

               rsprintf("</select> \n");
            }
         }
      }

      /* show button if JavaScript is switched off */
      rsprintf("<noscript>\n");
      rsprintf("<input type=submit value=\"%s\">\n", loc("Search"));
      rsprintf("</noscript>\n");

      /* show submit button for IE (otherwise <Return> will not work) */
      if (strstr(browser, "MSIE"))
         rsprintf("<input type=submit value=\"%s\">\n", loc("Search"));
   }

   rsprintf("&nbsp;<b>%d %s</b>&nbsp;", n_msg, loc("Entries"));

   rsprintf("</td></tr></table></td></tr>\n\n");
}