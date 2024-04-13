void show_elog_entry(LOGBOOK *lbs, char *dec_path, char *command) {
   int size, i, j, k, n, n_log, status, fh, length, message_error, index, n_hidden, message_id,
           orig_message_id, format_flags[MAX_N_ATTR], att_hide[MAX_ATTACHMENTS], att_inline[MAX_ATTACHMENTS],
           n_attachments, n_lines, n_disp_attr, attr_index[MAX_N_ATTR], thumb_status, max_n_lines;
   char str[2 * NAME_LENGTH], str2[NAME_LENGTH], ref[256], file_enc[256], attrib[MAX_N_ATTR][NAME_LENGTH];
   char date[80], text[TEXT_SIZE], menu_str[1000], cmd[256], script[256], orig_tag[80],
           reply_tag[MAX_REPLY_TO * 10], display[NAME_LENGTH], attachment[MAX_ATTACHMENTS][MAX_PATH_LENGTH],
           encoding[80], locked_by[256], att[256], lattr[256], mid[80], menu_item[MAX_N_LIST][NAME_LENGTH],
           format[80], slist[MAX_N_ATTR + 10][NAME_LENGTH], file_name[MAX_PATH_LENGTH],
           gattr[MAX_N_ATTR][NAME_LENGTH], svalue[MAX_N_ATTR + 10][NAME_LENGTH], *p,
           lbk_list[MAX_N_LIST][NAME_LENGTH], comment[256], class_name[80], class_value[80],
           fl[8][NAME_LENGTH], list[MAX_N_ATTR][NAME_LENGTH], domain[256], subdir[256], draft[256];
   FILE *f;
   BOOL first, show_text, display_inline, subtable, email, att_links;
   struct tm *pts;
   struct tm ts;
   struct stat st;
   time_t ltime, entry_ltime;

   message_id = atoi(dec_path);
   message_error = EL_SUCCESS;
   _current_message_id = message_id;
   email = (strstr(command, "email") != NULL);
   att_links = (strstr(command, "att-links") != NULL);

   /* check for custom form to display entry */
   if (getcfg(lbs->name, "Custom display form", str, sizeof(str))) {
      /* check if file starts with an absolute directory */
      if (str[0] == DIR_SEPARATOR || str[1] == ':')
         strcpy(file_name, str);
      else {
         strlcpy(file_name, logbook_dir, sizeof(file_name));
         strlcat(file_name, str, sizeof(file_name));
      }
      send_file_direct(str);
      return;
   }

   /* check for guest access */
   if (!getcfg(lbs->name, "Guest Menu commands", menu_str, sizeof(menu_str)) || logged_in(lbs))
      getcfg(lbs->name, "Menu commands", menu_str, sizeof(menu_str));

   /* default menu commands */
   if (menu_str[0] == 0) {
      strcpy(menu_str, "List, New, Edit, Delete, Reply, Duplicate, Find, ");

      if (getcfg(lbs->name, "Password file", str, sizeof(str))) {
         strcat(menu_str, "Config, Logout, ");
      } else {
         strcat(menu_str, "Config, ");
      }

      strcat(menu_str, "Help");
   } else {
      /* check for admin command */
      n = strbreak(menu_str, menu_item, MAX_N_LIST, ",", FALSE);
      menu_str[0] = 0;
      for (i = 0; i < n; i++) {
         if (strcmp(menu_item[i], "Admin") == 0) {
            if (!is_admin_user(lbs, getparam("unm")))
               continue;
         }
         strcat(menu_str, menu_item[i]);
         if (i < n - 1)
            strcat(menu_str, ", ");
      }
   }

   /*---- check next/previous message -------------------------------*/

   if (strieq(command, loc("Next")) || strieq(command, loc("Previous")) || strieq(command, loc("Last"))
       || strieq(command, loc("First"))) {
      orig_message_id = message_id;

      if (strieq(command, loc("Last")))
         message_id = el_search_message(lbs, EL_LAST, 0, FALSE);

      if (strieq(command, loc("First")))
         message_id = el_search_message(lbs, EL_FIRST, 0, FALSE);

      /* avoid display of "invalid id '0'", if "start page = 0?cmd=Last" */
      if (!message_id)
         dec_path[0] = 0;

      first = TRUE;
      do {
         if (strieq(command, loc("Next")))
            message_id = el_search_message(lbs, EL_NEXT, message_id, FALSE);

         if (strieq(command, loc("Previous")))
            message_id = el_search_message(lbs, EL_PREV, message_id, FALSE);

         if (!first) {
            if (strieq(command, loc("First")))
               message_id = el_search_message(lbs, EL_NEXT, message_id, FALSE);

            if (strieq(command, loc("Last")))
               message_id = el_search_message(lbs, EL_PREV, message_id, FALSE);
         } else
            first = FALSE;

         if (message_id == 0) {
            if (strieq(command, loc("Next")))
               message_error = EL_LAST_MSG;
            else
               message_error = EL_FIRST_MSG;

            message_id = orig_message_id;
            break;
         }

         size = sizeof(text);
         el_retrieve(lbs, message_id, date, attr_list, attrib, lbs->n_attr, text, &size, orig_tag, reply_tag,
                     attachment, encoding, locked_by, draft);

         /* check for locked attributes */
         for (i = 0; i < lbs->n_attr; i++) {
            sprintf(lattr, "l%s", attr_list[i]);
            if (isparam(lattr) == '1'
                && !(isparam(attr_list[i]) && strieq(getparam(attr_list[i]), attrib[i])))
               break;
         }
         if (i < lbs->n_attr)
            continue;

         /* check for attribute filter if not browsing */
         if (!isparam("browsing")) {
            for (i = 0; i < lbs->n_attr; i++) {
               if (isparam(attr_list[i]) && !(isparam(attr_list[i]) && strieq(getparam(attr_list[i]),
                                                                              attrib[i])))
                  break;
            }
            if (i < lbs->n_attr)
               continue;
         }

         sprintf(str, "%d", message_id);

         for (i = 0; i < lbs->n_attr; i++) {
            sprintf(lattr, "l%s", attr_list[i]);
            if (isparam(lattr) == '1') {
               if (strchr(str, '?') == NULL)
                  sprintf(str + strlen(str), "?%s=1", lattr);
               else
                  sprintf(str + strlen(str), "&amp;%s=1", lattr);
            }
         }

         redirect(lbs, str);
         return;

      } while (TRUE);
   }

   /*---- check for valid URL ---------------------------------------*/

   if (dec_path[0] && atoi(dec_path) == 0) {
      strencode2(str2, dec_path, sizeof(str2));
      sprintf(str, "%s: <b>%s</b>", loc("Invalid URL"), str2);
      show_error(str);
      return;
   }

   /*---- get current message ---------------------------------------*/

   if (message_id == 0)
      message_id = el_search_message(lbs, EL_LAST, 0, FALSE);

   status = 0;
   reply_tag[0] = orig_tag[0] = 0;
   if (message_id) {
      size = sizeof(text);
      status = el_retrieve(lbs, message_id, date, attr_list, attrib, lbs->n_attr, text, &size, orig_tag,
                           reply_tag, attachment, encoding, locked_by, draft);

      if (status != EL_SUCCESS)
         message_error = status;
      else {
         if (_logging_level > 2) {
            sprintf(str, "READ entry #%d", message_id);
            write_logfile(lbs, str);
         }
      }
   } else
      message_error = EL_EMPTY;

   /*---- check for conditional attribute ----*/

   evaluate_conditions(lbs, attrib);

   /*---- header ----*/

   /* header */
   if (status == EL_SUCCESS && message_error != EL_EMPTY) {
      str[0] = 0;

      if (getcfg(lbs->name, "Page Title", str, sizeof(str))) {
         i = build_subst_list(lbs, slist, svalue, attrib, TRUE);
         sprintf(mid, "%d", message_id);
         add_subst_list(slist, svalue, "message id", mid, &i);
         add_subst_time(lbs, slist, svalue, "entry time", date, &i, 0);
         strsubst_list(str, sizeof(str), slist, svalue, i);
         strip_html(str);
      } else
         strcpy(str, "ELOG");

      if (email) {
         /* embed CSS */
         show_html_header(lbs, FALSE, str, TRUE, FALSE, NULL, TRUE, 0);
         rsprintf("<body>\n");
      } else {
         sprintf(ref, "%d", message_id);
         strlcpy(script, "OnLoad=\"document.onkeypress=browse;\"", sizeof(script));
         if (str[0])
            show_standard_header(lbs, TRUE, str, ref, FALSE, NULL, script, 0);
         else
            show_standard_header(lbs, TRUE, lbs->name, ref, FALSE, NULL, script, 0);
      }
   } else
      show_standard_header(lbs, TRUE, "", "", FALSE, NULL, NULL, 0);

   /*---- title ----*/

   if (email)
      rsprintf("<table class=\"frame\" cellpadding=\"0\" cellspacing=\"0\">\n");
   else
      show_standard_title(lbs, "", 0);

   /*---- menu buttons ----*/

   if (!email) {

      rsprintf("<tr><td class=\"menuframe\">\n");
      rsprintf("<table width=\"100%%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
      rsprintf("<tr>\n");

      /*---- next/previous buttons ----*/

      if (!getcfg(lbs->name, "Enable browsing", str, sizeof(str)) || atoi(str) == 1) {
         rsprintf("<td class=\"menu1a\">\n");

         /* check if first.png exists, just put link there if not */
         strlcpy(file_name, resource_dir, sizeof(file_name));
         if (file_name[0] && file_name[strlen(file_name) - 1] != DIR_SEPARATOR)
            strlcat(file_name, DIR_SEPARATOR_STR, sizeof(file_name));
         strlcat(file_name, "themes", sizeof(file_name));
         strlcat(file_name, DIR_SEPARATOR_STR, sizeof(file_name));
         if (theme_name[0]) {
            strlcat(file_name, theme_name, sizeof(file_name));
            strlcat(file_name, DIR_SEPARATOR_STR, sizeof(file_name));
         }
         strlcat(file_name, "first.png", sizeof(file_name));
         if (stat(file_name, &st) >= 0) {
            rsprintf("<input type=image name=cmd_first alt=\"%s\"  title=\"%s\" src=\"first.png\">\n",
                     loc("First entry, Ctrl-Home"), loc("First entry, Ctrl-Home"));
            rsprintf("<input type=image name=cmd_previous alt=\"%s\"  title=\"%s\" src=\"previous.png\">\n",
                     loc("Previous entry, Ctrl-PgUp"), loc("Previous entry, Ctrl-PgUp"));
            rsprintf("<input type=image name=cmd_next alt=\"%s\" title=\"%s\" src=\"next.png\">\n",
                     loc("Next entry, Ctrl-PgDn"), loc("Next entry, Ctrl-PgDn"));
            rsprintf("<input type=image name=cmd_last alt=\"%s\" title=\"%s\" src=\"last.png\">\n",
                     loc("Last entry, Ctrl-End"), loc("Last entry, Ctrl-End"));
         } else {
            rsprintf("<a href=\"%d?cmd=%s\">|&lt;</a>&nbsp;\n", message_id, loc("First"));
            rsprintf("<a href=\"%d?cmd=%s\">&lt;</a>&nbsp;\n", message_id, loc("Previous"));
            rsprintf("<a href=\"%d?cmd=%s\">&gt;</a>&nbsp;\n", message_id, loc("Next"));
            rsprintf("<a href=\"%d?cmd=%s\">&gt;|</a>&nbsp;\n", message_id, loc("Last"));
         }

         rsprintf("</td>\n");
      }

      n = strbreak(menu_str, menu_item, MAX_N_LIST, ",", FALSE);

      rsprintf("<td class=\"menu1\">\n");

      for (i = 0; i < n; i++) {
         /* display menu item */
         strcpy(cmd, menu_item[i]);

         /* only display allowed commands */
         if (!is_user_allowed(lbs, cmd))
            continue;

         if (strieq(cmd, "Copy to") || strieq(cmd, "Move to")) {
            rsprintf("&nbsp;<input type=submit name=cmd value=\"%s\">\n", loc(cmd));
            if (strieq(cmd, "Copy to"))
               rsprintf("<select name=destc>\n");
            else
               rsprintf("<select name=destm>\n");

            if (getcfg(lbs->name, cmd, str, sizeof(str))) {
               n_log = strbreak(str, lbk_list, MAX_N_LIST, ",", FALSE);

               for (j = 0; j < n_log; j++)
                  rsprintf("<option value=\"%s\">%s\n", lbk_list[j], lbk_list[j]);
            } else {
               for (j = 0;; j++) {
                  if (!enumgrp(j, str))
                     break;

                  if (!is_logbook(str))
                     continue;

                  if (strieq(str, lbs->name))
                     continue;

                  rsprintf("<option value=\"%s\">%s\n", str, str);
               }
            }
            rsprintf("</select>\n");

            if (i < n - 1)
               rsprintf("&nbsp|\n");
         } else {
            strlcpy(str, loc(menu_item[i]), sizeof(str));
            url_encode(str, sizeof(str));

            if (strieq(menu_item[i], "list")) {
               if (getcfg(lbs->name, "Back to main", str, sizeof(str)) && atoi(str) == 1)
                  rsprintf("&nbsp;<a href=\"../\">%s</a>&nbsp;\n", loc(menu_item[i]));
               else
                  rsprintf("&nbsp;<a href=\".?id=%d\">%s</a>&nbsp;\n", message_id, loc(menu_item[i]));
            } else
               rsprintf("&nbsp;<a href=\"%d?cmd=%s\">%s</a>&nbsp;\n", message_id, str, loc(menu_item[i]));

            if (i < n - 1)
               rsprintf("|\n");
            else
               rsprintf("\n");
         }
      }

      rsprintf("</td>\n\n");

      rsprintf("</table></td></tr>\n\n");

      /*---- menu text ----*/

      if (getcfg(lbs->name, "menu text", str, sizeof(str))) {
         FILE *file;
         char filename[256], *buf;

         rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");

         /* check if file starts with an absolute directory */
         if (str[0] == DIR_SEPARATOR || str[1] == ':')
            strcpy(filename, str);
         else {
            strlcpy(filename, logbook_dir, sizeof(filename));
            strlcat(filename, str, sizeof(filename));
         }

         file = fopen(filename, "rb");
         if (file != NULL) {
            fseek(file, 0, SEEK_END);
            size = TELL(fileno(file));
            fseek(file, 0, SEEK_SET);

            buf = xmalloc(size + 1);
            fread(buf, 1, size, file);
            buf[size] = 0;
            fclose(file);

            rsputs(buf);

         } else
            rsprintf("<center><b>Error: file <i>\"%s\"</i> not found</b></center>", filename);

         rsprintf("</span></td></tr>");
      }
   }                            // if (!email)

   /*---- message ----*/

   if (reply_tag[0] || orig_tag[0])
      show_elog_thread(lbs, message_id, email, 0);

   if (message_error == EL_EMPTY)
      rsprintf("<tr><td class=\"errormsg\" colspan=2>%s</td></tr>\n", loc("Logbook is empty"));
   else if (message_error == EL_NO_MSG)
      rsprintf("<tr><td class=\"errormsg\" colspan=2>%s</td></tr>\n", loc("This entry has been deleted"));
   else {
      /* overall message table */
      rsprintf("<tr><td><table class=\"listframe\" width=\"100%%\" cellspacing=\"0\" cellpadding=\"0\">\n");

      /* check for locked attributes */
      for (i = 0; i < lbs->n_attr; i++) {
         sprintf(lattr, "l%s", attr_list[i]);
         if (isparam(lattr) == '1')
            break;
      }
      if (i < lbs->n_attr) {
         if (isparam(attr_list[i]))
            sprintf(str, " %s <i>\"%s = %s\"</i>", loc("with"), attr_list[i], getparam(attr_list[i]));
      } else
         str[0] = 0;

      if (message_error == EL_LAST_MSG)
         rsprintf("<tr><td class=\"notifymsg\" colspan=2>%s %s</td></tr>\n", loc("This is the last entry"),
                  str);

      if (message_error == EL_FIRST_MSG)
         rsprintf("<tr><td class=\"notifymsg\" colspan=2>%s %s</td></tr>\n", loc("This is the first entry"),
                  str);

      /* check for mail submissions */
      if (isparam("suppress")) {
         rsprintf("<tr><td class=\"notifymsg\" colspan=2>%s</td></tr>\n",
                  loc("Email notification suppressed"));
      } else if (isparam("error")) {
         strencode2(str, getparam("error"), sizeof(str));
         rsprintf("<tr><td class=\"errormsg\" colspan=2>%s</td></tr>\n", str);
      } else {
         for (i = 0;; i++) {
            sprintf(str, "mail%d", i);
            if (isparam(str)) {
               if (i == 0)
                  rsprintf("<tr><td class=\"notifymsg\" colspan=2>");
               strencode2(str, getparam(str), sizeof(str));
               rsprintf("%s <b>%s</b><br>\n", loc("Email sent to"), str);
            } else
               break;
         }
         if (i > 0)
            rsprintf("</tr>\n");
      }

      /*---- display message ID ----*/

      _current_message_id = message_id;

      if (email) {
         rsprintf("<tr><td class=\"title1\">\n");
         if (strstr(command, "oldemail") != NULL)
            rsprintf("%s:", loc("An old ELOG entry has been updated"));
         else
            rsprintf("%s:", loc("A new ELOG entry has been submitted"));
         rsprintf("</td></tr>\n");
      }

      if (locked_by[0]) {
         sprintf(str, "%s %s", loc("Entry is currently edited by"), locked_by);
         rsprintf
                 ("<tr><td nowrap colspan=2 class=\"errormsg\"><img src=\"stop.png\" alt=\"%s\"  title=\"%s\">\n",
                  loc("stop"), loc("stop"));
         rsprintf("%s.<br>%s.<br>%s.</td></tr>\n", str,
                  loc("You can \"steal\" the lock by editing this entry"),
                  loc("You might however then overwrite each other's modifications"));
      } else {
         if (draft[0]) {
            rsprintf("<tr><td nowrap colspan=2 class=\"errormsg\">%s\n",
                     loc("This is a draft message, edit and submit it to make it permanent"));
            rsprintf("&nbsp;<input type=button value=\"%s\" onClick=\"window.location.href='%d?cmd=%s';\">",
                     loc("Edit"), message_id, loc("Edit"));
            rsprintf("</td></tr>\n");
         }
      }

      rsprintf("<tr><td class=\"attribhead\">\n");

      for (i = 0; i < lbs->n_attr; i++) {
         strencode2(str, attrib[i], sizeof(str));
         rsprintf("<input type=hidden name=\"%s\" value=\"%s\">\n", attr_list[i], str);
      }

      /* browsing flag to distinguish "/../<attr>=<value>" from browsing */
      rsprintf("<input type=hidden name=browsing value=1>\n");

      if (getcfg(lbs->name, "ID display", display, sizeof(display))) {
         j = build_subst_list(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, attrib,
                              TRUE);
         sprintf(str, "%d", message_id);
         add_subst_list((char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, "message id", str, &j);
         add_subst_time(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, "entry time",
                        date, &j, 0);

         strsubst_list(display, sizeof(display), (char (*)[NAME_LENGTH]) slist,
                       (char (*)[NAME_LENGTH]) svalue, j);

      } else
         sprintf(display, "%d", message_id);

      if (email) {
         compose_base_url(lbs, str, sizeof(str), TRUE);
         sprintf(str + strlen(str), "%d", message_id);
         rsprintf("%s:&nbsp;<b>%s</b>&nbsp;&nbsp;", loc("Logbook"), lbs->name);
         rsprintf("%s:&nbsp;<a href=\"%s\"><b>%d</b></a>", loc("Message ID"), str, message_id);
      } else
         rsprintf("%s:&nbsp;<b>%s</b>\n", loc("Message ID"), display);

      /*---- display date ----*/

      if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
         strcpy(format, DEFAULT_TIME_FORMAT);

      entry_ltime = date_to_ltime(date);
      pts = localtime(&entry_ltime);
      assert(pts);
      my_strftime(str, sizeof(str), format, pts);

      rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;%s:&nbsp;<b>%s</b>\n", loc("Entry time"), str);

      /*---- link to original message or reply ----*/

      if (message_error != EL_FILE_ERROR && (reply_tag[0] || orig_tag[0])) {

         if (orig_tag[0]) {
            if (email)
               compose_base_url(lbs, ref, sizeof(ref), TRUE);
            else
               ref[0] = 0;
            sprintf(ref + strlen(ref), "%s", orig_tag);
            rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;%s:&nbsp;", loc("In reply to"));
            rsprintf("<b><a href=\"%s\">%s</a></b>\n", ref, orig_tag);
         }
         if (reply_tag[0]) {
            rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;%s:&nbsp;<b>", loc("Reply to this"));

            p = strtok(reply_tag, ",");
            do {
               if (email)
                  compose_base_url(lbs, ref, sizeof(ref), TRUE);
               else
                  ref[0] = 0;
               sprintf(ref + strlen(ref), "%s", p);
               rsprintf("<a href=\"%s\">%s</a>\n", ref, p);

               p = strtok(NULL, ",");

               if (p)
                  rsprintf("&nbsp; \n");

            } while (p);

            rsprintf("</b>\n");
         }
      }

      rsprintf("</td></tr>\n");

      /*---- display attributes ----*/

      /* retrieve attribute flags */
      for (i = 0; i < lbs->n_attr; i++) {
         format_flags[i] = 0;
         sprintf(str, "Format %s", attr_list[i]);
         if (getcfg(lbs->name, str, format, sizeof(format))) {
            n = strbreak(format, fl, 8, ",", FALSE);
            if (n > 0)
               format_flags[i] = atoi(fl[0]);
         }
      }

      /* 2 column table for all attributes */
      rsprintf("<tr><td><table width=\"100%%\" cellpadding=\"0\" cellspacing=\"0\">");
      subtable = 0;

      /* generate list of attributes to show */
      if (getcfg(lbs->name, "Show attributes", str, sizeof(str))) {
         n_disp_attr = strbreak(str, list, MAX_N_ATTR, ",", FALSE);
         for (i = 0; i < n_disp_attr; i++) {
            for (j = 0; j < lbs->n_attr; j++)
               if (strieq(attr_list[j], list[i]))
                  break;
            if (!strieq(attr_list[j], list[i]))
               /* attribute not found */
               j = 0;
            attr_index[i] = j;
         }
      } else {
         for (i = 0; i < lbs->n_attr; i++)
            attr_index[i] = i;
         n_disp_attr = lbs->n_attr;
      }

      for (j = 0; j < n_disp_attr; j++) {

         i = attr_index[j];
         if (getcfg(lbs->name, "Password file", str, sizeof(str)) && getcfg(lbs->name, "Guest display", str,
                                                                            sizeof(str)) && !isparam("unm")) {

            n = strbreak(str, gattr, MAX_N_ATTR, ",", FALSE);
            for (k = 0; k < n; k++)
               if (strieq(gattr[k], attr_list[i]))
                  break;

            if (k == n)
               continue;
         }

         strcpy(class_name, "attribname");
         strcpy(class_value, "attribvalue");

         sprintf(str, "Format %s", attr_list[i]);
         if (getcfg(lbs->name, str, format, sizeof(format))) {
            n = strbreak(format, fl, 8, ",", FALSE);
            if (n > 1)
               strlcpy(class_name, fl[1], sizeof(class_name));
            if (n > 2)
               strlcpy(class_value, fl[2], sizeof(class_value));
         }

         if (format_flags[i] & AFF_SAME_LINE)
            /* if attribute on same line, do nothing */
            rsprintf("");
         else if (i < lbs->n_attr - 1 && (format_flags[i + 1] & AFF_SAME_LINE)) {
            /* if next attribute on same line, start a new subtable */
            rsprintf("<tr><td colspan=2><table width=\"100%%\" cellpadding=\"0\" cellspacing=\"0\"><tr>");
            subtable = 1;
         } else
            /* for normal attribute, start new row */
            rsprintf("<tr>");

         sprintf(lattr, "l%s", attr_list[i]);

         /* display cell with optional tooltip */
         sprintf(str, "Tooltip %s", attr_list[i]);
         if (getcfg(lbs->name, str, comment, sizeof(comment)))
            rsprintf("<td nowrap title=\"%s\" class=\"%s\">", comment, class_name);
         else
            rsprintf("<td nowrap class=\"%s\">", class_name);

         if (getcfg(lbs->name, "Filtered browsing", str, sizeof(str)) && atoi(str) == 1) {
            if (isparam(lattr) == '1')
               rsprintf("<input type=\"checkbox\" checked name=\"%s\" value=\"1\">&nbsp;", lattr);
            else
               rsprintf("<input alt=\"text\" title=\"text\"type=\"checkbox\" name=\"%s\" value=\"1\">&nbsp;",
                        lattr);
         }

         /* display checkbox for boolean attributes */
         if (strieq(attr_options[i][0], "boolean")) {
            if (atoi(attrib[i]) == 1)
               rsprintf("%s:</td><td class=\"%s\"><input type=checkbox checked disabled></td>\n",
                        attr_list[i], class_value);
            else
               rsprintf("%s:</td><td class=\"%s\"><input type=checkbox disabled></td>\n", attr_list[i],
                        class_value);
         }
            /* display image for icon */
         else if (attr_flags[i] & AF_ICON) {
            rsprintf("%s:</td><td class=\"%s\">\n", attr_list[i], class_value);
            if (attrib[i][0]) {
               sprintf(str, "Icon comment %s", attrib[i]);
               getcfg(lbs->name, str, comment, sizeof(comment));

               if (comment[0])
                  rsprintf("<img src=\"icons/%s\" alt=\"%s\" title=\"%s\">", attrib[i], comment, comment);
               else
                  rsprintf("<img src=\"icons/%s\" alt=\"%s\" title=\"%s\">", attrib[i], attrib[i], attrib[i]);
            }
            rsprintf("&nbsp;</td>\n");
         } else if ((attr_flags[i] & (AF_MULTI | AF_MUSERLIST | AF_MUSEREMAIL)) && (format_flags[i]
                                                                                    & AFF_MULTI_LINE)) {
            rsprintf("%s:</td><td class=\"%s\">\n", attr_list[i], class_value);

            /* separate options into individual lines */
            strlcpy(str, attrib[i], sizeof(str));
            p = strtok(str, "|");
            while (p) {
               while (*p == ' ')
                  p++;

               rsputs2(lbs, email, p);

               p = strtok(NULL, "|");

               if (p)
                  rsprintf("<br>");
            }

            rsprintf("</td>\n");
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

            rsprintf("%s:</td><td class=\"%s\">%s&nbsp;</td>\n", attr_list[i], class_value, str);

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

            rsprintf("%s:</td><td class=\"%s\">%s&nbsp;</td>\n", attr_list[i], class_value, str);

         } else {
            rsprintf("%s:</td><td class=\"%s\">\n", attr_list[i], class_value);

            sprintf(str, "Change %s", attr_list[i]);
            if (getcfg(lbs->name, str, display, sizeof(display))) {
               k = build_subst_list(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue,
                                    attrib, TRUE);
               sprintf(str, "%d", message_id);
               add_subst_list((char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, "message id",
                              str, &k);
               add_subst_time(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue,
                              "entry time", date, &k, attr_flags[i]);

               strsubst_list(display, sizeof(display), (char (*)[NAME_LENGTH]) slist,
                             (char (*)[NAME_LENGTH]) svalue, k);

            } else
               strcpy(display, attrib[i]);

            if (is_html(display) && !is_script(display) && html_allowed(lbs))
               rsputs(display);
            else
               rsputs2(lbs, email, display);

            rsprintf("&nbsp;</td>\n");
         }

         if (i < lbs->n_attr - 1 && (format_flags[i + 1] & AFF_SAME_LINE) == 0) {
            /* if next attribute not on same line, close row or subtable */
            if (subtable) {
               rsprintf("</table></td></tr>\n");
               subtable = 0;
            } else
               rsprintf("</tr>");
         }

         /* if last attribute, close row or subtable */
         if (i == lbs->n_attr - 1) {
            if (subtable) {
               rsprintf("</table></td></tr>\n");
               subtable = 0;
            } else
               rsprintf("</tr>");
         }
      }

      rsputs("</table></td></tr>\n");   // 2 column table

      rsputs("</table><!-- listframe -->\n");

      rsputs("</td></tr>\n");

      /*---- message text ----*/

      show_text = !getcfg(lbs->name, "Show text", str, sizeof(str)) || atoi(str) == 1;

      if (getcfg(lbs->name, "Password file", str, sizeof(str)) && getcfg(lbs->name, "Guest display", str,
                                                                         sizeof(str)) && !isparam("unm")) {

         n = strbreak(str, gattr, MAX_N_ATTR, ",", FALSE);
         for (j = 0; j < n; j++)
            if (strieq(gattr[j], "text"))
               break;

         if (j == n)
            show_text = FALSE;
      }

      if (show_text) {
         rsprintf("<tr><td class=\"messageframe\">");

         if (strieq(encoding, "html")) {
            if (email)
               replace_inline_img(lbs, text);
            rsputs(text);
         } else if (strieq(encoding, "ELCode")) {
            rsputs_elcode(lbs, email, text);
         } else {
            rsputs("<pre class=\"messagepre\">");
            rsputs2(lbs, email, text);
            rsputs("</pre>");
         }

         rsputs("</td></tr>\n");

         n_hidden = 0;
         for (i = 0, n_attachments = 0; i < MAX_ATTACHMENTS; i++) {
            att_inline[i] = 0;
            att_hide[i] = getcfg(lbs->name, "Show attachments", str, sizeof(str)) && atoi(str) == 0;

            if (is_inline_attachment(encoding, message_id, text, i, attachment[i]))
               att_inline[i] = 1;

            if (attachment[i][0])
               n_attachments++;
         }

         if (isparam("hide")) {
            strlcpy(str, getparam("hide"), sizeof(str));
            p = strtok(str, ",");
            while (p != NULL) {
               if (atoi(p) < MAX_ATTACHMENTS) {
                  att_hide[atoi(p)] = 1;
                  n_hidden++;
               }
               p = strtok(NULL, ",");
            }
         }
         if (isparam("show")) {
            strlcpy(str, getparam("show"), sizeof(str));
            p = strtok(str, ",");
            while (p != NULL) {
               if (atoi(p) < MAX_ATTACHMENTS) {
                  att_hide[atoi(p)] = 0;
               }
               p = strtok(NULL, ",");
            }
         }

         for (index = 0; index < MAX_ATTACHMENTS; index++) {
            if (attachment[index][0] && strlen(attachment[index]) > 14 && !att_inline[index]) {
               for (i = 0; i < (int) strlen(attachment[index]); i++)
                  att[i] = toupper(attachment[index][i]);
               att[i] = 0;

               /* determine size of attachment */
               strlcpy(file_name, lbs->data_dir, sizeof(file_name));
               generate_subdir_name(attachment[index], subdir, sizeof(subdir));
               strlcat(file_name, subdir, sizeof(file_name));
               strlcat(file_name, attachment[index], sizeof(file_name));
               thumb_status = create_thumbnail(lbs, file_name);

               length = 0;
               fh = open(file_name, O_RDONLY | O_BINARY);
               if (fh > 0) {
                  lseek(fh, 0, SEEK_END);
                  length = TELL(fh);
                  close(fh);
               }

               strlcpy(str, attachment[index], sizeof(str));
               str[13] = 0;
               strcpy(file_enc, attachment[index] + 14);
               url_encode(file_enc, sizeof(file_enc));  /* for file names with special characters like "+" */
               if (email && !att_links) {
                  retrieve_domain(domain, sizeof(domain));
                  sprintf(ref, "cid:att%d@%s", index, domain);
               } else if (email) {
                  compose_base_url(lbs, ref, sizeof(ref), TRUE);
                  sprintf(ref + strlen(ref), "%s", str);
                  sprintf(ref + strlen(ref), "/%s", file_enc);
               } else {
                  sprintf(ref, "%s/%s", str, file_enc);
               }

               /* overall table */
               rsprintf("<tr><td><table class=\"listframe\" width=\"100%%\" cellspacing=0>\n");

               rsprintf("<tr><td nowrap width=\"10%%\" class=\"attribname\">%s %d:</td>\n",
                        loc("Attachment"), index + 1);

               if (email && !att_links)
                  rsprintf("<td class=\"attribvalue\">%s\n", attachment[index] + 14);
               else
                  rsprintf("<td class=\"attribvalue\"><a href=\"%s\" target=\"_blank\">%s</a>\n", ref,
                           attachment[index] + 14);

               rsprintf("&nbsp;<span class=\"bytes\">");

               if (length < 1024)
                  rsprintf("%d Bytes", length);
               else if (length < 1024 * 1024)
                  rsprintf("%d kB", length / 1024);
               else
                  rsprintf("%1.3lf MB", length / 1024.0 / 1024.0);

               rsprintf("</span>\n");

               /* retrieve submission date */
               memset(&ts, 0, sizeof(ts));
               ts.tm_mon = (attachment[index][2] - '0') * 10 + attachment[index][3] - '0' - 1;
               ts.tm_mday = (attachment[index][4] - '0') * 10 + attachment[index][5] - '0';
               ts.tm_year = (attachment[index][0] - '0') * 10 + attachment[index][1] - '0';

               ts.tm_hour = (attachment[index][7] - '0') * 10 + attachment[index][8] - '0';
               ts.tm_min = (attachment[index][9] - '0') * 10 + attachment[index][10] - '0';
               ts.tm_sec = (attachment[index][11] - '0') * 10 + attachment[index][12] - '0';

               if (ts.tm_year < 90)
                  ts.tm_year += 100;
               ltime = mktime(&ts);

               /* show upload date/time only if different from entry date/time */
               if (abs((int) (ltime - entry_ltime)) > 3600) {
                  if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
                     strcpy(format, DEFAULT_TIME_FORMAT);

                  my_strftime(str, sizeof(str), format, &ts);

                  rsprintf("&nbsp;<span class=\"uploaded\">");
                  rsprintf("Uploaded %s", str);
                  rsprintf("</span>\n");
               }

               /* determine if displayed inline */
               display_inline = is_image(file_name) || is_ascii(file_name);
               if (chkext(att, ".PS") || chkext(att, ".PDF"))
                  display_inline = 0;
               if ((chkext(att, ".HTM") || chkext(att, ".HTML")) && is_full_html(file_name))
                  display_inline = 0;
               if (thumb_status)
                  display_inline = 1;

               if (display_inline) {
                  /* hide this / show this */
                  if (!email) {
                     rsprintf("<span class=\"bytes\">");

                     rsprintf("&nbsp;|&nbsp;");
                     if (att_hide[index]) {
                        rsprintf("<a href=\"%d?hide=", message_id);
                        for (i = 0; i < MAX_ATTACHMENTS; i++)
                           if (att_hide[i] && i != index) {
                              rsprintf("%d,", i);
                           }
                        rsprintf("&amp;show=%d", index);
                        rsprintf("\">%s</a>", loc("Show"));
                     } else {
                        rsprintf("<a href=\"%d?hide=", message_id);
                        for (i = 0; i < MAX_ATTACHMENTS; i++)
                           if (att_hide[i] || i == index) {
                              rsprintf("%d,", i);
                           }
                        rsprintf("\">%s</a>", loc("Hide"));
                     }

                     /* hide all */
                     if (n_hidden < n_attachments) {
                        rsprintf("&nbsp;|&nbsp;<a href=\"%d?hide=", message_id);
                        for (i = 0; i < MAX_ATTACHMENTS; i++)
                           if (attachment[i][0]) {
                              rsprintf("%d,", i);
                           }
                        rsprintf("\">%s</a>", loc("Hide all"));
                     }

                     /* show all */
                     if (n_hidden > 0) {
                        for (i = 0; i < MAX_ATTACHMENTS; i++)
                           if (att_hide[i])
                              break;
                        if (i < MAX_ATTACHMENTS) {
                           rsprintf("&nbsp;|&nbsp;<a href=\"%d?show=", message_id);
                           for (i = 0; i < MAX_ATTACHMENTS; i++)
                              if (att_hide[i])
                                 rsprintf("%d,", i);
                           rsprintf("\">%s</a>", loc("Show all"));
                        }
                     }

                     rsprintf("</span>\n");
                  }
               }

               rsprintf("</td></tr></table></td></tr>\n");
               strlcpy(file_name, lbs->data_dir, sizeof(file_name));
               generate_subdir_name(attachment[index], subdir, sizeof(subdir));
               strlcat(file_name, subdir, sizeof(file_name));
               strlcat(file_name, attachment[index], sizeof(file_name));

               if (!att_hide[index] && display_inline) {

                  if (thumb_status) {
                     rsprintf("<tr><td class=\"attachmentframe\">\n");

                     if (thumb_status == 3) {
                        rsprintf("<font color=red><b>%s</b></font>\n",
                                 loc("Cannot create thumbnail, please check ImageMagick installation"));
                     } else {
                        if (thumb_status == 2 && !email) {
                           for (i = 0;; i++) {
                              strlcpy(str, file_name, sizeof(str));
                              if (chkext(file_name, ".pdf") || chkext(file_name, ".ps"))
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
                           if (!email) {
                              rsprintf("<a name=\"att%d\" href=\"%s\">\n", index + 1, ref);
                              strlcpy(str, ref, sizeof(str));
                              if (chkext(file_name, ".pdf") || chkext(file_name, ".ps"))
                                 if (strrchr(str, '.'))
                                    *strrchr(str, '.') = 0;
                              strlcat(str, ".png", sizeof(str));
                              rsprintf("<img src=\"%s\" alt=\"%s\" title=\"%s\"></a>\n", str,
                                       attachment[index] + 14, attachment[index] + 14);
                           }
                        }
                     }
                     rsprintf("</td></tr>\n\n");
                  } else if (is_image(att)) {
                     if (!email) {
                        rsprintf("<tr><td class=\"attachmentframe\">\n");
                        rsprintf("<a name=\"att%d\"></a>\n", index + 1);
                        rsprintf("<img src=\"%s\" alt=\"%s\" title=\"%s\">\n", ref, attachment[index] + 14,
                                 attachment[index] + 14);
                        rsprintf("</td></tr>\n\n");
                     }
                  } else {
                     if (is_ascii(file_name)) {
                        /* display attachment */
                        rsprintf("<tr><td class=\"messageframe\">\n");

                        /* anchor for references */
                        rsprintf("<a name=\"att%d\"></a>\n", index + 1);

                        if (!chkext(att, ".HTML"))
                           rsprintf("<pre class=\"messagepre\">");

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
                                 if (!chkext(att, ".HTML"))
                                    rsputs2(lbs, email, str);
                                 else
                                    rsputs(str);
                              }
                              n_lines++;
                           }
                           fclose(f);
                        }

                        if (!chkext(att, ".HTML"))
                           rsprintf("</pre>");
                        rsprintf("\n");
                        if (max_n_lines == 0)
                           rsprintf("<i><b>%d lines</b></i>\n", n_lines);
                        else if (n_lines > max_n_lines)
                           rsprintf("<i><b>... %d more lines ...</b></i>\n", n_lines - max_n_lines);

                        rsprintf("</td></tr>\n");
                     }
                  }
               }
            }
         }
      }
   }

   /* overall table (class "frame" from show_standard_header) */
   rsprintf("\r\n</table><!-- show_standard_title -->\r\n");

   show_bottom_text(lbs);
   if (!email)
      rsprintf("</form>\n");
   rsprintf("</body></html>\r\n");
}