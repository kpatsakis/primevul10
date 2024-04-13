void xml_import(LOGBOOK *lbs, const char *xml, const char *xmlfile) {
   char str[NAME_LENGTH], date[80], error[256], encoding[256], *list, *p, in_reply_to[80],
           reply_to[MAX_REPLY_TO * 10], attachment[MAX_ATTACHMENTS][MAX_PATH_LENGTH], attachment_all[64
                                                                                                     *
                                                                                                     MAX_ATTACHMENTS];
   int i, j, index, n_attr, iline, n_imported, i_line, line_len, message_id, bedit;
   time_t ltime;
   PMXML_NODE root, entry;

   iline = n_imported = 0;

   n_attr = lbs->n_attr;

   root = mxml_parse_buffer(xml, error, sizeof(error), NULL);
   if (root == NULL) {
      strencode2(str, error, sizeof(str));
      show_error(str);
      return;
   }

   root = mxml_find_node(root, "ELOG_LIST");
   if (root == NULL) {
      sprintf(str, loc("XML file does not contain %s element"), "&lt;ELOG_LIST&gt;");
      show_error(str);
      return;
   }

   entry = mxml_subnode(root, 0);

   if (mxml_find_node(entry, "MID") == NULL) {
      sprintf(str, loc("XML file does not contain %s element"), "&lt;MID&gt;");
      show_error(str);
      return;
   }

   if (mxml_find_node(entry, "DATE") == NULL) {
      sprintf(str, loc("XML file does not contain %s element"), "&lt;DATE&gt;");
      show_error(str);
      return;
   }

   if (mxml_find_node(entry, "ENCODING") == NULL) {
      sprintf(str, loc("XML file does not contain %s element"), "&lt;ENCODING&gt;");
      show_error(str);
      return;
   }

   if (isparam("preview")) {

      /* title row */
      sprintf(str, loc("XML import preview of %s"), xmlfile);
      show_standard_header(lbs, TRUE, str, "./", FALSE, NULL, NULL, 0);
      rsprintf("<table class=\"frame\" cellpadding=\"0\" cellspacing=\"0\">\n");
      rsprintf("<tr><td class=\"title1\">%s</td></tr>\n", str, str);

      /* menu buttons */
      rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Cancel"));
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("XML Import"));

      /* hidden fields */
      if (isparam("head"))
         rsprintf("<input type=hidden name=head value=\"%s\">\n", getparam("head"));
      if (isparam("keep"))
         rsprintf("<input type=hidden name=keep value=\"%s\">\n", getparam("keep"));
      rsprintf("<input type=hidden name=xmlfile value=\"%s\">\n", xmlfile);

      rsprintf("</span></td></tr>\n\n");
      rsprintf("<tr><td><table class=\"listframe\" width=\"100%%\" cellspacing=0>");
   }

   list = (char *) xmalloc(MAX_N_ATTR * NAME_LENGTH);

   /* derive attributes from XML file */
   if (isparam("head")) {
      if (isparam("preview")) {
         rsprintf("<tr>\n");
         for (i = 0; i < mxml_get_number_of_children(entry); i++) {
            strlcpy(str, mxml_get_name(mxml_subnode(entry, i)), sizeof(str));
            if (strieq(str, "MID"))
               strcpy(str, "ID");
            if (strieq(str, "DATE"))
               strcpy(str, loc("Date"));
            if (strieq(str, "TEXT"))
               strcpy(str, loc("Text"));
            if (!strieq(str, "ENCODING") && !strieq(str, "IN_REPLY_TO") && !strieq(str, "REPLY_TO")
                && !strieq(str, "ATTACHMENT"))
               rsprintf("<th class=\"listtitle\">%s</th>\n", str);
         }

         rsprintf("</tr>\n");
         n_attr = i;
      } else {
         for (i = j = 0; i < mxml_get_number_of_children(entry); i++) {
            strlcpy(str, mxml_get_name(mxml_subnode(entry, i)), NAME_LENGTH);
            if (stricmp(str, "MID") != 0 && stricmp(str, "DATE") != 0 &&
                stricmp(str, "ENCODING") != 0 && stricmp(str, "TEXT") != 0 &&
                stricmp(str, "IN_REPLY_TO") != 0 && stricmp(str, "REPLY_TO") != 0 &&
                stricmp(str, "ATTACHMENT") != 0)
               strlcpy(attr_list[j++], mxml_get_name(mxml_subnode(entry, i)), NAME_LENGTH);
         }

         if (!set_attributes(lbs, attr_list, j))
            return;
         lbs->n_attr = n_attr = j;
      }
   } else {
      if (isparam("preview")) {
         rsprintf("<tr>\n");
         rsprintf("<th class=\"listtitle\">%s</th>\n", "ID");
         rsprintf("<th class=\"listtitle\">%s</th>\n", loc("Date"));
         for (i = 0; i < n_attr; i++)
            rsprintf("<th class=\"listtitle\">%s</th>\n", attr_list[i]);
         rsprintf("<th class=\"listtitle\">%s</th>\n", loc("Text"));
         rsprintf("</tr>\n");
      }
   }

   for (index = 0; index < mxml_get_number_of_children(root); index++) {

      entry = mxml_subnode(root, index);

      if (isparam("preview")) {
         rsprintf("<tr>\n");
         for (i = 0; i < mxml_get_number_of_children(entry); i++) {

            strlcpy(str, mxml_get_name(mxml_subnode(entry, i)), NAME_LENGTH);
            if (strieq(str, "ENCODING") || strieq(str, "IN_REPLY_TO") || strieq(str, "REPLY_TO")
                || strieq(str, "ATTACHMENT"))
               continue;
            if (strieq(str, "TEXT"))
               break;

            if (iline % 2 == 0)
               rsputs("<td class=\"list1\">");
            else
               rsputs("<td class=\"list2\">");

            strlcpy(str, mxml_get_value(mxml_subnode(entry, i)), NAME_LENGTH);

            if (!str[0])
               rsputs("&nbsp;");
            else
               rsputs(str);

            rsputs("</td>\n");
         }

         rsputs("<td class=\"summary\">");
         if (mxml_find_node(entry, "TEXT")) {
            strlcpy(str, mxml_get_value(mxml_find_node(entry, "TEXT")), sizeof(str));
            if (str[0]) {

               /* limit output to 3 lines */
               for (i = i_line = line_len = 0; i < (int) sizeof(str) - 1; i++, line_len++) {
                  if (str[i] == '\n') {
                     i_line++;
                     line_len = 0;
                  } else
                     /* limit line length to 150 characters */
                  if (line_len > 150 && str[i] == ' ') {
                     str[i] = '\n';
                     i_line++;
                     line_len = 0;
                  }

                  if (i_line == 3)
                     break;
               }
               str[i] = 0;

               strip_html(str);
               if (str[0])
                  strencode(str);
               else
                  rsputs("&nbsp;");
            } else
               rsputs("&nbsp;");
         }
         rsputs("</td>\n");
         rsputs("</tr>\n");
         iline++;

      } else {

         message_id = 0;
         if (isparam("keep"))
            message_id = atoi(mxml_get_value(mxml_find_node(entry, "MID")));

         for (i = 0; i < n_attr; i++) {
            strlcpy(str, attr_list[i], sizeof(str));
            while (strchr(str, ' '))
               *strchr(str, ' ') = '_';
            if (mxml_find_node(entry, str) == NULL)
               *(list + (i * NAME_LENGTH)) = 0;
            else
               strlcpy(list + i * NAME_LENGTH, mxml_get_value(mxml_find_node(entry, str)), NAME_LENGTH);
         }

         /* interprete date entries correctly */
         for (i = 0; i < n_attr; i++) {
            if (attr_flags[i] & AF_DATE) {
               /* convert to seconds in Unix format */
               ltime = convert_date(list + i * NAME_LENGTH);
               if (ltime == 0) {
                  show_error(loc("Invalid date format"));
                  return;
               }
               sprintf(list + i * NAME_LENGTH, "%d", (int) ltime);
            }
            if (attr_flags[i] & AF_DATETIME) {
               /* convert to seconds in Unix format */
               ltime = convert_datetime(list + i * NAME_LENGTH);
               if (ltime == 0) {
                  show_error(loc("Invalid date format"));
                  return;
               }
               sprintf(list + i * NAME_LENGTH, "%d", (int) ltime);
            }
         }

         encoding[0] = 0;
         if (mxml_find_node(entry, "ENCODING"))
            strlcpy(encoding, mxml_get_value(mxml_find_node(entry, "ENCODING")), sizeof(encoding));
         else
            strcpy(encoding, "plain");

         reply_to[0] = 0;
         if (mxml_find_node(entry, "REPLY_TO"))
            strlcpy(reply_to, mxml_get_value(mxml_find_node(entry, "REPLY_TO")), sizeof(reply_to));

         in_reply_to[0] = 0;
         if (mxml_find_node(entry, "IN_REPLY_TO"))
            strlcpy(in_reply_to, mxml_get_value(mxml_find_node(entry, "IN_REPLY_TO")), sizeof(in_reply_to));

         date[0] = 0;
         if (mxml_find_node(entry, "DATE"))
            strlcpy(date, mxml_get_value(mxml_find_node(entry, "DATE")), sizeof(date));

         attachment_all[0] = 0;
         if (mxml_find_node(entry, "ATTACHMENT"))
            strlcpy(attachment_all, mxml_get_value(mxml_find_node(entry, "ATTACHMENT")),
                    sizeof(attachment_all));
         memset(attachment, 0, sizeof(attachment));
         for (i = 0; i < MAX_ATTACHMENTS; i++) {
            if (i == 0)
               p = strtok(attachment_all, ",");
            else
               p = strtok(NULL, ",");

            if (p != NULL)
               strlcpy(attachment[i], p, MAX_PATH_LENGTH);
            else
               break;
         }

         str[0] = 0;
         if (mxml_find_node(entry, "TEXT"))
            p = mxml_get_value(mxml_find_node(entry, "TEXT"));
         else
            p = str;

         bedit = FALSE;
         if (isparam("keep")) {
            for (i = 0; i < *lbs->n_el_index; i++)
               if (lbs->el_index[i].message_id == message_id)
                  break;
            if (lbs->el_index[i].message_id == message_id)
               bedit = TRUE;
         }

         /* submit entry */
         if (el_submit
                 (lbs, message_id, bedit, date, attr_list, (char (*)[NAME_LENGTH]) list, n_attr, p,
                  in_reply_to,
                  reply_to, encoding, attachment, FALSE, NULL, NULL))
            n_imported++;
      }
   }

   xfree(list);

   if (isparam("preview")) {
      rsprintf("</table></td></tr></table>\n");
      show_bottom_text(lbs);
      rsprintf("</form></body></html>\r\n");

      return;
   }

   sprintf(str, loc("%d entries successfully imported"), n_imported);
   show_elog_list(lbs, 0, 0, 0, TRUE, str);
}