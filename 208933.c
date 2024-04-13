void display_line(LOGBOOK *lbs, int message_id, int number, char *mode, int expand, int level,
                  BOOL printable, int n_line, int show_attachments, int show_att_column,
                  char *date, char *in_reply_to, char *reply_to, int n_attr_disp,
                  char disp_attr[MAX_N_ATTR + 4][NAME_LENGTH], BOOL disp_attr_link[MAX_N_ATTR + 4],
                  char attrib[MAX_N_ATTR][NAME_LENGTH], int n_attr, char *text, BOOL show_text,
                  char attachment[MAX_ATTACHMENTS][MAX_PATH_LENGTH], char *encoding, BOOL select,
                  int *n_display, char *locked_by, int highlight, regex_t *re_buf, int highlight_mid,
                  int absolute_link, char *draft) {
   char str[NAME_LENGTH], ref[256], *nowrap, rowstyle[80], tdstyle[80], format[256],
           file_name[MAX_PATH_LENGTH], *slist, *svalue, comment[256], param[80], subdir[256];
   char display[NAME_LENGTH], attr_icon[80];
   int i, j, n, i_line, index, colspan, n_attachments, line_len, thumb_status, max_line_len, n_lines,
           max_n_lines;
   BOOL skip_comma;
   FILE *f;
   struct tm *pts;
   time_t ltime;

   slist = (char *) xmalloc((MAX_N_ATTR + 10) * NAME_LENGTH);
   svalue = (char *) xmalloc((MAX_N_ATTR + 10) * NAME_LENGTH);

   _current_message_id = message_id;
   ref[0] = 0;
   if (absolute_link)
      compose_base_url(lbs, ref, sizeof(ref), FALSE);
   sprintf(ref + strlen(ref), "../%s/%d", lbs->name_enc, message_id);

   if (strieq(mode, "Summary")) {
      if (draft && draft[0])
         strcpy(rowstyle, "listdraft");
      else if (highlight_mid == message_id) {
         if (number % 2 == 1)
            strcpy(rowstyle, "list1h");
         else
            strcpy(rowstyle, "list2h");
      } else {
         if (number % 2 == 1)
            strcpy(rowstyle, "list1");
         else
            strcpy(rowstyle, "list2");
      }
   } else if (strieq(mode, "Full")) {
      if (highlight_mid == message_id)
         strcpy(rowstyle, "list1h");
      else
         strcpy(rowstyle, "list1");
   } else if (strieq(mode, "Threaded")) {
      if (draft && draft[0])
         strcpy(rowstyle, "threaddraft");
      else if (highlight) {
         if (highlight == message_id)
            strcpy(rowstyle, "thread");
         else
            strcpy(rowstyle, "threadreply");
      } else {
         if (highlight_mid == message_id) {
            if (level == 0)
               strcpy(rowstyle, "threadh");
            else
               strcpy(rowstyle, "threadreply");
         } else {
            if (level == 0)
               strcpy(rowstyle, "thread");
            else
               strcpy(rowstyle, "threadreply");
         }
      }
   }

   rsprintf("<tr>");

   /* check attributes for row style */
   for (i = 0; i < n_attr; i++) {
      if (attrib[i][0] == 0)
         sprintf(str, "Style %s \"\"", attr_list[i]);
      else
         sprintf(str, "Style %s %s", attr_list[i], attrib[i]);
      if (getcfg(lbs->name, str, display, sizeof(display))) {
         sprintf(str, "%s\" style=\"%s", rowstyle, display);
         strlcpy(rowstyle, str, sizeof(rowstyle));
         break;
      }
   }

   /* only single cell for threaded display */
   if (strieq(mode, "Threaded")) {
      rsprintf("<td align=left class=\"%s\">", rowstyle);

      if (locked_by && locked_by[0]) {
         sprintf(str, "%s %s", loc("Entry is currently edited by"), locked_by);
         rsprintf("<img src=\"stop.png\" alt=\"%s\" title=\"%s\">&nbsp;", str, str);
      }

      /* show select box */
      if (select && level == 0)
         rsprintf("<input type=checkbox name=\"s%d\" value=%d>\n", (*n_display)++, message_id);

      for (i = 0; i < level; i++)
         rsprintf("&nbsp;&nbsp;&nbsp;");

      /* display "+" if expandable */
      if (expand == 0 && (reply_to[0] || in_reply_to[0]))
         rsprintf("+&nbsp;");
   }

   nowrap = printable ? "" : "nowrap";
   skip_comma = FALSE;

   if (getcfg(lbs->name, "List conditions", str, sizeof(str)) && atoi(str) == 1)
      evaluate_conditions(lbs, attrib);

   if (strieq(mode, "Threaded") && getcfg(lbs->name, "Thread display", display, sizeof(display))) {
      /* check if to use icon from attributes */
      attr_icon[0] = 0;
      if (getcfg(lbs->name, "Thread icon", attr_icon, sizeof(attr_icon))) {
         for (i = 0; i < n_attr; i++)
            if (strieq(attr_list[i], attr_icon))
               break;
         if (i < n_attr && attrib[i][0])
            strcpy(attr_icon, attrib[i]);
         else
            attr_icon[0] = 0;
      }

      if (highlight != message_id)
         rsprintf("\n<a href=\"%s\">", ref);

      if (attr_icon[0])
         rsprintf("\n<img border=0 src=\"icons/%s\" alt=\"%s\" title=\"%s\">\n&nbsp;", attr_icon, attr_icon,
                  attr_icon);
      else {
         /* display standard icons */
         if (level == 0)
            rsprintf("\n<img border=0 src=\"entry.png\" alt=\"%s\" title=\"%s\">\n&nbsp;", loc("Entry"),
                     loc("Entry"));
         else
            rsprintf("\n<img border=0 src=\"reply.png\" alt=\"%s\" title=\"%s\">\n&nbsp;", loc("Reply"),
                     loc("Reply"));
      }
      if (highlight != message_id)
         rsprintf("</a>\n");

      j = build_subst_list(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, attrib, TRUE);
      sprintf(str, "%d", message_id);
      add_subst_list((char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, "message id", str, &j);
      add_subst_time(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, "entry time", date,
                     &j, 0);

      strsubst_list(display, sizeof(display), (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue,
                    j);

      if (highlight != message_id)
         rsprintf("\n<a href=\"%s\">\n", ref);
      else
         rsprintf("\n<b>");


      if (is_html(display) && !is_script(display) && html_allowed(lbs))
         rsputs(display);
      else
         rsputs2(lbs, absolute_link, display);

      rsputs("&nbsp;");
      for (i = n = 0; i < MAX_ATTACHMENTS; i++)
         if (attachment && attachment[i][0])
            n++;
      if (n > 5) {
         if (highlight != message_id)
            rsprintf("<a href=\"%s\">", ref);
         rsprintf("<b>%dx", n);
         rsprintf("<img border=\"0\" align=\"absmiddle\" src=\"attachment.png\"></b></a>");
      } else {
         for (i = 0; i < MAX_ATTACHMENTS; i++)
            if (attachment && attachment[i][0]) {
               strlcpy(str, attachment[i], sizeof(str));
               str[13] = 0;
               sprintf(ref, "../%s/%s/%s", lbs->name, str, attachment[i] + 14);
               url_encode(ref, sizeof(ref));    /* for file names with special characters like "+" */

               rsprintf("<a href=\"%s\" target=\"_blank\">", ref);
               rsprintf
                       ("<img border=\"0\" align=\"absmiddle\" src=\"attachment.png\" alt=\"%s\" title=\"%s\"></a>",
                        attachment[i] + 14, attachment[i] + 14);
            }
      }

      if (highlight != message_id)
         rsprintf("</a>\n", ref);
      else
         rsprintf("</b>\n");
   } else {
      /* show select box */
      if (select && !strieq(mode, "Threaded")) {
         rsprintf("<td class=\"%s\">", rowstyle);
         rsprintf("<input type=checkbox name=\"s%d\" value=%d>\n", (*n_display)++, message_id);
         rsprintf("</td>\n");
      }

      if (strieq(mode, "Threaded")) {
         if (highlight != message_id)
            rsprintf("\n<a href=\"%s\">\n", ref);
         else
            rsprintf("\n<b>");
      }

      skip_comma = TRUE;
      for (index = 0; index < n_attr_disp; index++) {
         if (strieq(disp_attr[index], loc("ID"))) {
            if (strieq(mode, "Threaded")) {
               if (level == 0)
                  rsprintf("\n<img border=0 src=\"entry.png\" alt=\"%s\" title=\"%s\">&nbsp;", loc("Entry"),
                           loc("Entry"));
               else
                  rsprintf("\n<img border=0 src=\"reply.png\" alt=\"%s\" title=\"%s\">&nbsp;", loc("Reply"),
                           loc("Reply"));

            } else {

               rsprintf("<td class=\"%s\">", rowstyle);

               if (locked_by && locked_by[0]) {
                  sprintf(str, "%s %s", loc("Entry is currently edited by"), locked_by);
                  rsprintf("\n<img src=\"stop.png\" alt=\"%s\" title=\"%s\">&nbsp;", str, str);
               }

               if (draft && draft[0]) {
                  strlcpy(display, loc("Draft"), sizeof(display));
               } else {
                  if (getcfg(lbs->name, "ID display", display, sizeof(display))) {
                     j = build_subst_list(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue,
                                          attrib, TRUE);
                     sprintf(str, "%d", message_id);
                     add_subst_list((char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue,
                                    "message id",
                                    str, &j);
                     add_subst_time(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue,
                                    "entry time", date, &j, 0);

                     strsubst_list(display, sizeof(display), (char (*)[NAME_LENGTH]) slist,
                                   (char (*)[NAME_LENGTH]) svalue, j);

                  } else
                     sprintf(display, "%d", message_id);
               }

               rsprintf("\n<a href=\"%s\">&nbsp;&nbsp;%s&nbsp;&nbsp;</a>\n", ref, display);
               rsprintf("</td>\n");
            }
         }

         if (strieq(disp_attr[index], loc("Logbook"))) {
            if (strieq(mode, "Threaded")) {
               if (skip_comma) {
                  rsprintf(" %s", lbs->name);
                  skip_comma = FALSE;
               } else
                  rsprintf(", %s", lbs->name);
            } else {
               if (disp_attr_link == NULL || disp_attr_link[index])
                  rsprintf("\n<td class=\"%s\" %s><a href=\"%s\">%s</a></td>\n", rowstyle, nowrap, ref,
                           lbs->name);
               else
                  rsprintf("\n<td class=\"%s\" %s>%s</td>\n", rowstyle, nowrap, lbs->name);
            }
         }

         if (strieq(disp_attr[index], loc("Edit"))) {
            if (!strieq(mode, "Threaded")) {
               rsprintf("\n<td class=\"%s\" %s><a href=\"%s?cmd=%s\">", rowstyle, nowrap, ref, loc("Edit"));
               rsprintf("\n<img src=\"edit.png\" border=0 alt=\"%s\" title=\"%s\"></a></td>\n",
                        loc("Edit entry"), loc("Edit entry"));
            }
         }

         if (strieq(disp_attr[index], loc("Delete"))) {
            if (!strieq(mode, "Threaded")) {
               rsprintf("\n<td class=\"%s\" %s><a href=\"%s?cmd=%s\">", rowstyle, nowrap, ref, loc("Delete"));
               rsprintf("\n<img src=\"delete.png\" border=0 alt=\"%s\" title=\"%s\"></a></td>\n",
                        loc("Delete entry"), loc("Delete entry"));
            }
         }

         if (strieq(disp_attr[index], loc("Date"))) {
            if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
               strcpy(format, DEFAULT_TIME_FORMAT);

            ltime = date_to_ltime(date);
            pts = localtime(&ltime);
            assert(pts);
            my_strftime(str, sizeof(str), format, pts);

            if (strieq(mode, "Threaded")) {
               if (skip_comma) {
                  rsprintf(" %s", str);
                  skip_comma = FALSE;
               } else
                  rsprintf(", %s", str);
            } else {
               if (disp_attr_link == NULL || disp_attr_link[index])
                  rsprintf("\n<td class=\"%s\" %s><a href=\"%s\">%s</a></td>\n", rowstyle, nowrap, ref, str);
               else
                  rsprintf("\n<td class=\"%s\" %s>%s</td>\n", rowstyle, nowrap, str);
            }
         }

         for (i = 0; i < n_attr; i++)
            if (strieq(disp_attr[index], attr_list[i])) {

               /* check attributes for cell style */
               strlcpy(tdstyle, rowstyle, sizeof(tdstyle));
               sprintf(str, "Cell Style %s %s", attr_list[i], attrib[i]);
               if (getcfg(lbs->name, str, display, sizeof(display))) {
                  sprintf(str, "%s\" style=\"%s", rowstyle, display);
                  strlcpy(tdstyle, str, sizeof(rowstyle));
               }

               if (strieq(mode, "Threaded")) {
                  if (strieq(attr_options[i][0], "boolean")) {
                     if (atoi(attrib[i]) == 1) {
                        if (skip_comma) {
                           rsprintf(" ");
                           skip_comma = FALSE;
                        } else
                           rsprintf(", ");

                        if (is_html(attrib[i]) && !is_script(attrib[i]) && html_allowed(lbs))
                           rsputs(attrib[i]);
                        else
                           rsputs2(lbs, absolute_link, attrib[i]);

                        rsprintf("&nbsp");
                     }
                  } else if (attr_flags[i] & AF_DATE) {
                     if (skip_comma) {
                        rsprintf(" ");
                        skip_comma = FALSE;
                     } else
                        rsprintf(", ");

                     sprintf(str, "Date format %s", attr_list[i]);
                     if (!getcfg(lbs->name, str, format, sizeof(format)))
                        if (!getcfg(lbs->name, "Date format", format, sizeof(format)))
                           strcpy(format, DEFAULT_DATE_FORMAT);

                     ltime = atoi(attrib[i]);
                     pts = localtime(&ltime);
                     assert(pts);
                     if (ltime == 0)
                        strcpy(str, "-");
                     else
                        my_strftime(str, sizeof(str), format, pts);

                     rsputs(str);
                  } else if (attr_flags[i] & AF_DATETIME) {
                     if (skip_comma) {
                        rsprintf(" ");
                        skip_comma = FALSE;
                     } else
                        rsprintf(", ");

                     sprintf(str, "Time format %s", attr_list[i]);
                     if (!getcfg(lbs->name, str, format, sizeof(format)))
                        if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
                           strcpy(format, DEFAULT_TIME_FORMAT);

                     ltime = atoi(attrib[i]);
                     pts = localtime(&ltime);
                     assert(pts);
                     if (ltime == 0)
                        strcpy(str, "-");
                     else
                        my_strftime(str, sizeof(str), format, pts);

                     rsputs(str);
                  } else if (attr_flags[i] & AF_ICON) {

                     sprintf(str, "Icon comment %s", attrib[i]);
                     getcfg(lbs->name, str, comment, sizeof(comment));
                     if (!comment[0])
                        strcpy(comment, attrib[i]);

                     if (attrib[i][0])
                        rsprintf("&nbsp;\n<img border=0 src=\"icons/%s\" alt=\"%s\" title=\"%s\">&nbsp;",
                                 attrib[i], comment, comment);
                  } else {
                     if (skip_comma) {
                        rsprintf(" ");
                        skip_comma = FALSE;
                     } else
                        rsprintf(", ");

                     if (is_html(attrib[i]) && !is_script(attrib[i]) && html_allowed(lbs))
                        rsputs(attrib[i]);
                     else
                        rsputs2(lbs, absolute_link, attrib[i]);

                  }
               } else {
                  if (strieq(attr_options[i][0], "boolean")) {
                     if (atoi(attrib[i]) == 1)
                        rsprintf("\n<td class=\"%s\"><input type=checkbox checked disabled></td>\n", tdstyle);
                     else
                        rsprintf("\n<td class=\"%s\"><input type=checkbox disabled></td>\n", tdstyle);
                  } else if (attr_flags[i] & AF_DATE) {

                     sprintf(str, "Date format %s", attr_list[i]);
                     if (!getcfg(lbs->name, str, format, sizeof(format)))
                        if (!getcfg(lbs->name, "Date format", format, sizeof(format)))
                           strcpy(format, DEFAULT_DATE_FORMAT);

                     ltime = atoi(attrib[i]);
                     pts = localtime(&ltime);
                     assert(pts);
                     if (ltime == 0)
                        strcpy(str, "-");
                     else
                        my_strftime(str, sizeof(str), format, pts);

                     if (disp_attr_link == NULL || disp_attr_link[index])
                        rsprintf("\n<td class=\"%s\" %s><a href=\"%s\">%s</a></td>\n", tdstyle, nowrap, ref,
                                 str);
                     else
                        rsprintf("\n<td class=\"%s\" %s>%s</td>\n", tdstyle, nowrap, str);
                  } else if (attr_flags[i] & AF_DATETIME) {

                     sprintf(str, "Time format %s", attr_list[i]);
                     if (!getcfg(lbs->name, str, format, sizeof(format)))
                        if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
                           strcpy(format, DEFAULT_TIME_FORMAT);

                     ltime = atoi(attrib[i]);
                     pts = localtime(&ltime);
                     assert(pts);
                     if (ltime == 0)
                        strcpy(str, "-");
                     else
                        my_strftime(str, sizeof(str), format, pts);

                     if (disp_attr_link == NULL || disp_attr_link[index])
                        rsprintf("\n<td class=\"%s\" %s><a href=\"%s\">%s</a></td>\n", tdstyle, nowrap, ref,
                                 str);
                     else
                        rsprintf("\n<td class=\"%s\" %s>%s</td>\n", tdstyle, nowrap, str);
                  } else if (attr_flags[i] & AF_ICON) {
                     rsprintf("<td class=\"%s\">", rowstyle);

                     sprintf(str, "Icon comment %s", attrib[i]);
                     getcfg(lbs->name, str, comment, sizeof(comment));
                     if (!comment[0])
                        strcpy(comment, attrib[i]);

                     if (attrib[i][0])
                        rsprintf("\n<img border=0 src=\"icons/%s\" alt=\"%s\" title=\"%s\">", attrib[i],
                                 comment, comment);

                     rsprintf("&nbsp;</td>");
                  } else {
                     rsprintf("<td class=\"%s\">", tdstyle);

                     if (disp_attr_link == NULL || disp_attr_link[index])
                        rsprintf("<a href=\"%s\">", ref);

                     sprintf(str, "List Change %s", attr_list[i]);
                     if (getcfg(lbs->name, str, display, sizeof(display))) {
                        j = build_subst_list(lbs, (char (*)[NAME_LENGTH]) slist,
                                             (char (*)[NAME_LENGTH]) svalue, attrib, TRUE);
                        sprintf(str, "%d", message_id);
                        add_subst_list((char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue,
                                       "message id", str, &j);
                        add_subst_time(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue,
                                       "entry time", date, &j, 0);

                        strsubst_list(display, sizeof(display), (char (*)[NAME_LENGTH]) slist,
                                      (char (*)[NAME_LENGTH]) svalue, j);

                     } else
                        strcpy(display, attrib[i]);

                     if (is_html(display) && !is_script(display) && html_allowed(lbs))
                        rsputs(display);
                     else {
                        if (isparam(attr_list[i])) {
                           highlight_searchtext(re_buf + 1 + i, display, str, TRUE);
                           strlcpy(display, str, sizeof(display));
                        } else if (isparam("subtext") && isparam("sall") && atoi(getparam("sall"))) {
                           highlight_searchtext(re_buf, display, str, TRUE);
                           strlcpy(display, str, sizeof(display));
                        }
                        rsputs2(lbs, absolute_link, display);
                     }

                     if (disp_attr_link == NULL || disp_attr_link[index])
                        rsprintf("</a>");

                     /* at least one space to produce non-empty table cell */
                     if (!display[0])
                        rsprintf("&nbsp;");

                     rsprintf("</td>");
                  }
               }
            }
      }

      if (strieq(mode, "Threaded")) {

         rsputs("&nbsp;");

         for (i = n = 0; i < MAX_ATTACHMENTS; i++)
            if (attachment && attachment[i][0])
               n++;
         if (n > 5) {
            if (highlight != message_id)
               rsprintf("<a href=\"%s\">", ref);
            rsprintf("<b>%dx", n);
            rsprintf("<img border=\"0\" align=\"absmiddle\" src=\"attachment.png\"></b></a>");
         } else {
            for (i = 0; i < MAX_ATTACHMENTS; i++)
               if (attachment && attachment[i][0]) {
                  strlcpy(str, attachment[i], sizeof(str));
                  str[13] = 0;
                  sprintf(ref, "../%s/%s/%s", lbs->name, str, attachment[i] + 14);
                  url_encode(ref, sizeof(ref)); /* for file names with special characters like "+" */

                  rsprintf("<a href=\"%s\" target=\"_blank\">", ref);
                  rsprintf
                          ("<img border=\"0\" align=\"absmiddle\" src=\"attachment.png\" alt=\"%s\" title=\"%s\"></a>",
                           attachment[i] + 14, attachment[i] + 14);
               }
         }

         if (highlight != message_id)
            rsprintf("</a>\n");
         else
            rsprintf("</b>\n");
      }
   }

   if (strieq(mode, "Threaded") && expand > 1 && show_text) {
      rsprintf("</td></tr>\n");

      rsprintf("<tr><td class=\"summary\">");

      if (expand == 2) {
         for (i = i_line = line_len = 0; i < (int) sizeof(str) - 1; i++, line_len++) {
            str[i] = text[i];
            if (line_break(text + i, encoding)) {
               i_line++;
               line_len = 0;
            } else
               /* limit line length to 150 characters */
            if (line_len > 150 && text[i] == ' ') {
               str[i] = '\n';
               i_line++;
               line_len = 0;
            }

            if (i_line == n_line)
               break;
         }
         str[i] = 0;

         /* only show text, not to rip apart HTML documents,
            e.g. only the start of a table */
         if (strieq(encoding, "HTML"))
            strip_html(str);
         if (str[0])
            strencode_nouml(str);
         else
            rsputs("&nbsp;");
      } else {
         if (strieq(encoding, "plain")) {
            rsputs("<pre>");
            if (text[0])
               rsputs2(lbs, absolute_link, text);
            else
               rsputs("&nbsp;");
            rsputs("</pre>");
         } else if (strieq(encoding, "ELCode"))
            rsputs_elcode(lbs, FALSE, text);
         else if (text[0])
            rsputs(text);
         else
            rsputs("&nbsp;");
      }

      rsprintf("</td></tr>\n");
   }

   if (strieq(mode, "Summary") && n_line > 0 && show_text) {
      rsprintf("<td class=\"summary\">");

      if (getcfg(lbs->name, "Summary line length", param, sizeof(param)))
         max_line_len = atoi(param);
      else
         max_line_len = n_line >= 10 ? 140 : 40;
      for (i = i_line = line_len = 0; i < (int) sizeof(str) - 1; line_len++, i++) {
         str[i] = text[i];
         if (line_break(text + i, encoding)) {
            i_line++;
            line_len = 0;
         } else
            /* limit line length to max_line_len characters */
         if (line_len > max_line_len && text[i] == ' ') {
            str[i] = '\n';
            i_line++;
            line_len = 0;
         }

         if (i_line == n_line)
            break;
      }
      str[i] = 0;

      /* only show text, not to rip apart HTML documents,
         e.g. only the start of a table */
      if (strieq(encoding, "HTML"))
         strip_html(str);
      if (str[0])
         strencode_nouml(str);
      else
         rsputs("&nbsp;");

      rsputs("</td>\n");
   }

   if (show_att_column) {
      /* show attachment icons */
      rsputs("<td class=\"listatt\">&nbsp;");

      for (i = n = 0; i < MAX_ATTACHMENTS; i++)
         if (attachment && attachment[i][0])
            n++;
      if (n > 5) {
         if (highlight != message_id)
            rsprintf("<a href=\"%s\">", ref);
         rsprintf("<b>%dx", n);
         rsprintf("<img border=\"0\" align=\"absmiddle\" src=\"attachment.png\"></b></a>");
      } else {
         for (i = 0; i < MAX_ATTACHMENTS; i++)
            if (attachment && attachment[i][0]) {
               strlcpy(str, attachment[i], sizeof(str));
               str[13] = 0;
               sprintf(ref, "../%s/%s/%s", lbs->name, str, attachment[i] + 14);
               url_encode(ref, sizeof(ref));    /* for file names with special characters like "+" */

               rsprintf("<a href=\"%s\" target=\"_blank\">", ref);
               rsprintf
                       ("<img border=\"0\" align=\"absmiddle\" src=\"attachment.png\" alt=\"%s\" title=\"%s\"></a>",
                        attachment[i] + 14, attachment[i] + 14);
            }
      }

      rsputs("&nbsp;</td>");
   }

   colspan = n_attr_disp;

   if (select)
      colspan++;

   if (strieq(mode, "Full") && show_text) {
      if (!getcfg(lbs->name, "Show text", str, sizeof(str)) || atoi(str) == 1) {
         rsprintf("<tr><td class=\"messagelist\" colspan=%d>", colspan);

         if (strieq(encoding, "plain")) {
            rsputs("<pre>");
            rsputs2(lbs, absolute_link, text);
            rsputs("</pre>");
         } else if (strieq(encoding, "ELCode"))
            rsputs_elcode(lbs, FALSE, text);
         else
            rsputs(text);

         rsprintf("</td></tr>\n");
      }

      /* count number of attachments */
      n_attachments = 0;
      if (show_attachments) {
         for (index = 0; index < MAX_ATTACHMENTS; index++) {
            if (attachment[index][0]) {
               /* check if attachment is inlined */
               sprintf(str, "[img]elog:/%d[/img]", index + 1);
               if (strieq(encoding, "ELCode") && stristr(text, str))
                  continue;

               n_attachments++;
            }
         }
      }

      for (index = 0; index < MAX_ATTACHMENTS; index++) {
         if (show_attachments && attachment[index][0]) {

            /* check if attachment is inlined */
            if (is_inline_attachment(encoding, message_id, text, index, attachment[index]))
               continue;

            strlcpy(str, attachment[index], sizeof(str));
            str[13] = 0;
            sprintf(ref, "../%s/%s/%s", lbs->name, str, attachment[index] + 14);
            url_encode(ref, sizeof(ref));       /* for file names with special characters like "+" */

            strlcpy(file_name, lbs->data_dir, sizeof(file_name));
            generate_subdir_name(attachment[index], subdir, sizeof(subdir));
            strlcat(file_name, subdir, sizeof(file_name));
            strlcat(file_name, attachment[index], sizeof(file_name));
            thumb_status = create_thumbnail(lbs, file_name);

            if (!show_attachments) {
               rsprintf("<a href=\"%s\" target=\"_blank\">%s</a>&nbsp;&nbsp;&nbsp;&nbsp; ", ref,
                        attachment[index] + 14);
            } else {

               if (thumb_status) {
                  rsprintf
                          ("<tr><td colspan=%d class=\"attachment\">%s %d: <a href=\"%s\" target=\"_blank\">%s</a>\n",
                           colspan, loc("Attachment"), index + 1, ref, attachment[index] + 14);

                  if (show_attachments) {
                     rsprintf("<tr><td colspan=%d class=\"attachmentframe\">\n", colspan);
                     if (thumb_status == 2) {
                        for (i = 0;; i++) {
                           strlcpy(str, file_name, sizeof(str));
                           if (chkext(str, ".pdf") || chkext(str, ".ps"))
                              if (strrchr(str, '.'))
                                 *strrchr(str, '.') = 0;
                           sprintf(str + strlen(str), "-%d.png", i);
                           if (file_exist(str)) {
                              strlcpy(str, ref, sizeof(str));
                              if (chkext(file_name, ".pdf") || chkext(file_name, ".ps"))
                                 if (strrchr(str, '.'))
                                    *strrchr(str, '.') = 0;
                              sprintf(str + strlen(str), "-%d.png", i);
                              rsprintf("<a name=\"att%d\" href=\"%s\">\n", index + 1, ref);
                              rsprintf("<img src=\"%s\" alt=\"%s\" title=\"%s\"></a>\n", str,
                                       attachment[index] + 14, attachment[index] + 14);
                           } else
                              break;
                        }
                     } else {
                        rsprintf("<a name=\"att%d\" href=\"%s\">\n", index + 1, ref);
                        strlcpy(str, ref, sizeof(str));
                        if (chkext(str, ".pdf") || chkext(str, ".ps"))
                           if (strrchr(str, '.'))
                              *strrchr(str, '.') = 0;
                        strlcat(str, ".png", sizeof(str));
                        rsprintf("<img src=\"%s\" alt=\"%s\" title=\"%s\"></a>\n", str, attachment[index]
                                                                                        + 14,
                                 attachment[index] + 14);
                     }
                     rsprintf("</td></tr>\n\n");
                  }
               } else {
                  if (is_image(attachment[index])) {
                     rsprintf
                             ("<tr><td colspan=%d class=\"attachment\">%s %d: <a href=\"%s\" target=\"_blank\">%s</a>\n",
                              colspan, loc("Attachment"), index + 1, ref, attachment[index] + 14);
                     if (show_attachments) {
                        rsprintf("</td></tr><tr>");
                        rsprintf("<td colspan=%d class=\"attachmentframe\">", colspan);
                        rsprintf("<img src=\"%s\" alt=\"%s\" title=\"%s\">", ref, attachment[index] + 14,
                                 attachment[index] + 14);
                        rsprintf("</td></tr>\n");
                     }
                  } else {
                     rsprintf
                             ("<tr><td colspan=%d class=\"attachment\">%s %d: <a href=\"%s\" target=\"_blank\">%s</a>\n",
                              colspan, loc("Attachment"), index + 1, ref, attachment[index] + 14);

                     strlcpy(file_name, lbs->data_dir, sizeof(file_name));
                     generate_subdir_name(attachment[index], subdir, sizeof(subdir));
                     strlcat(file_name, subdir, sizeof(file_name));
                     strlcat(file_name, attachment[index], sizeof(file_name));

                     if (is_ascii(file_name) && !chkext(attachment[index], ".PS")
                         && !chkext(attachment[index], ".PDF") && !chkext(attachment[index], ".EPS")
                         && !chkext(attachment[index], ".SVG")
                         && !chkext(attachment[index], ".HTM") && !chkext(attachment[index], ".HTML")
                         && show_attachments) {

                        rsprintf("</td></tr>\n");

                        /* display attachment */
                        rsprintf("<tr><td colspan=%d class=\"messageframe\">\n", colspan);

                        /* anchor for references */
                        rsprintf("<a name=\"att%d\"></a>\n", index + 1);

                        /* display attachment */

                        if (!chkext(attachment[index], ".HTML"))
                           rsprintf("<pre class=\"messagepre\">");

                        strlcpy(file_name, lbs->data_dir, sizeof(file_name));
                        generate_subdir_name(attachment[index], subdir, sizeof(subdir));
                        strlcat(file_name, subdir, sizeof(file_name));
                        strlcat(file_name, attachment[index], sizeof(file_name));

                        f = fopen(file_name, "rt");

                        n_lines = 0;
                        if (getcfg(lbs->name, "Attachment lines", str, sizeof(str)))
                           max_n_lines = atoi(str);
                        else
                           max_n_lines = 300;

                        if (f != NULL) {
                           while (!feof(f)) {
                              str[0] = 0;
                              fgets(str, sizeof(str), f);

                              if (n_lines < max_n_lines) {
                                 if (!chkext(attachment[index], ".HTML"))
                                    rsputs2(lbs, absolute_link, str);
                                 else
                                    rsputs(str);
                              }
                              n_lines++;
                           }
                           fclose(f);
                        }

                        if (!chkext(attachment[index], ".HTML"))
                           rsprintf("</pre>");
                        rsprintf("\n");
                        if (max_n_lines == 0)
                           rsprintf("<i><b>%d lines</b></i>\n", n_lines);
                        else if (n_lines > max_n_lines)
                           rsprintf("<i><b>... %d more lines ...</b></i>\n", n_lines - max_n_lines);
                     }
                     rsprintf("</td></tr>\n");
                  }
               }
            }
         }
      }
   }

   xfree(slist);
   xfree(svalue);
}