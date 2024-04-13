void csv_import(LOGBOOK *lbs, const char *csv, const char *csvfile) {
   const char *p;
   char *line, *list;
   char str[256], date[80], sep[80];
   int i, j, n, n_attr, iline, n_imported, textcol, datecol, attr_offset;
   BOOL first, in_quotes, filltext;
   time_t ltime;

   list = (char *) xmalloc((MAX_N_ATTR + 2) * NAME_LENGTH);
   line = (char *) xmalloc(10000);

   first = TRUE;
   in_quotes = FALSE;
   iline = n_imported = 0;
   filltext = FALSE;
   textcol = -1;
   datecol = -1;
   attr_offset = 0;

   strcpy(sep, ",");
   if (isparam("sep"))
      strcpy(sep, getparam("sep"));
   if (sep[0] == 0)
      strcpy(sep, ",");
   if (strieq(sep, "auto")) {

      /* count commas */
      for (i = 0, p = csv; p; i++) {
         p = strchr(p, ',');
         if (p)
            p++;
      }
      n = i;

      /* count semicolon */
      for (i = 0, p = csv; p; i++) {
         p = strchr(p, ';');
         if (p)
            p++;
      }

      strcpy(sep, i > n ? ";" : ",");
   }

   n_attr = lbs->n_attr;

   if (isparam("preview")) {

      /* title row */
      sprintf(str, loc("CSV import preview of %s"), csvfile);
      show_standard_header(lbs, TRUE, str, "./", FALSE, NULL, NULL, 0);
      rsprintf("<table class=\"frame\" cellpadding=\"0\" cellspacing=\"0\">\n");
      rsprintf("<tr><td class=\"title1\">%s</td></tr>\n", str, str);

      /* menu buttons */
      rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Cancel"));
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("CSV Import"));

      /* hidden fields */
      rsprintf("<input type=hidden name=sep value=\"%s\">\n", sep);
      if (isparam("head"))
         rsprintf("<input type=hidden name=head value=\"%s\">\n", getparam("head"));
      if (isparam("notignore"))
         rsprintf("<input type=hidden name=notignore value=\"%s\">\n", getparam("notignore"));
      if (isparam("filltext"))
         rsprintf("<input type=hidden name=filltext value=\"%s\">\n", getparam("filltext"));
      rsprintf("<input type=hidden name=csvfile value=\"%s\">\n", csvfile);

      rsprintf("</span></td></tr>\n\n");

      rsprintf("<tr><td><table class=\"listframe\" width=\"100%%\" cellspacing=0>");
   }

   p = csv;
   datecol = -1;
   attr_offset = 0;

   do {
      for (i = 0; i < 10000 && *p; i++) {
         if (!in_quotes && (*p == '\r' || *p == '\n'))
            break;

         line[i] = *p++;
         if (line[i] == '"')
            in_quotes = !in_quotes;
      }

      line[i] = 0;
      while (*p == '\r' || *p == '\n')
         p++;
      if (!*p)
         break;

      memset(list, 0, MAX_N_ATTR * NAME_LENGTH);
      n = strbreak(line, (char (*)[NAME_LENGTH]) list, MAX_N_ATTR, sep, FALSE);

      if (n == MAX_N_ATTR) {
         strlcpy(str, loc("Too many attributes in CSV file"), sizeof(str));
         show_error(str);
      }

      /* check if text column is present */
      if (first && isparam("filltext") && atoi(getparam("filltext"))) {
         for (i = 0; i < n; i++)
            if (strieq(list + i * NAME_LENGTH, loc("text"))) {
               filltext = TRUE;
               textcol = i;
               break;
            }
      }

      /* interprete date entries correctly */
      if (!(first && isparam("head"))) {
         for (i = attr_offset; i < n; i++) {
            if (attr_flags[i - attr_offset] & AF_DATE) {
               /* convert to seconds in Unix format */
               ltime = convert_date(list + i * NAME_LENGTH);
               if (ltime == 0) {
                  show_error(loc("Invalid date format"));
                  return;
               }
               sprintf(list + i * NAME_LENGTH, "%d", (int) ltime);
            }
            if (attr_flags[i - attr_offset] & AF_DATETIME) {
               /* convert to seconds in Unix format */
               ltime = convert_datetime(list + i * NAME_LENGTH);
               if (ltime == 0) {
                  show_error(loc("Invalid date format"));
                  return;
               }
               sprintf(list + i * NAME_LENGTH, "%d", (int) ltime);
            }
         }
      }

      if (first) {
         /* check for date column */
         for (i = attr_offset = 0; i < n; i++)
            if (strieq(list + i * NAME_LENGTH, "Date"))
               datecol = i;

         /* skip message ID */
         for (i = attr_offset = 0; i < n; i++)
            if (strieq(list + i * NAME_LENGTH, "Message ID") || strieq(list + i * NAME_LENGTH, "Date"))
               attr_offset++;
      }

      /* derive attributes from first line */
      if (first && isparam("head")) {

         if (isparam("preview")) {
            rsprintf("<tr>\n");
            for (i = attr_offset; i < n; i++)
               if (i != textcol)
                  rsprintf("<th class=\"listtitle\">%s</th>\n", list + i * NAME_LENGTH);

            if (filltext)
               rsprintf("<th class=\"listtitle\">%s</th>\n", loc("text"));

            rsprintf("</tr>\n");

            if (filltext)
               n_attr = n - 1 - attr_offset;
            else
               n_attr = n - attr_offset;

         } else {
            for (i = j = attr_offset; i < n; i++)
               if (i != textcol)
                  strlcpy(attr_list[j++ - attr_offset], list + i * NAME_LENGTH, NAME_LENGTH);

            if (filltext) {
               if (!set_attributes(lbs, attr_list, n - 1 - attr_offset))
                  return;
               lbs->n_attr = n - 1 - attr_offset;
            } else {
               if (!set_attributes(lbs, attr_list, n - attr_offset))
                  return;
               lbs->n_attr = n - attr_offset;
            }
            n_attr = lbs->n_attr;
         }

      } else {

         /* ignore first line */
         if (first && !isparam("notignore")) {
            first = FALSE;
            continue;
         }

         if (isparam("preview")) {
            rsprintf("<tr>\n");
            for (i = j = attr_offset; i < n_attr + attr_offset; i++) {
               if (iline % 2 == 0)
                  rsputs("<td class=\"list1\">");
               else
                  rsputs("<td class=\"list2\">");

               /* skip text column */
               if (i == textcol)
                  j++;

               if (i >= n || !list[j * NAME_LENGTH])
                  rsputs("&nbsp;");
               else
                  rsputs(list + j * NAME_LENGTH);

               rsputs("</td>\n");
               j++;
            }

            if (filltext) {
               rsputs("<td class=\"summary\">");
               if (list[textcol * NAME_LENGTH])
                  rsputs(list + textcol * NAME_LENGTH);
               else
                  rsputs("&nbsp;");
               rsputs("</td>\n");
            }

            rsputs("</tr>\n");
            iline++;

         } else {

            /* get date and check it */
            if (datecol != -1) {
               strlcpy(date, list + datecol * NAME_LENGTH, sizeof(date));
               ltime = date_to_ltime(date);
               if (ltime <= 0) {
                  /* try other date formats */
                  ltime = convert_datetime(date);
                  if (ltime <= 0)
                     ltime = convert_date(date);
                  if (ltime <= 0) {
                     strcpy(str, loc("Invalid date format"));
                     strlcat(str, ": \"", sizeof(str));
                     strlcat(str, date, sizeof(str));
                     strlcat(str, "\"", sizeof(str));
                     show_error(str);
                     return;
                  }
                  /* convert back ltime to date */
                  get_rfc2822_date(date, sizeof(date), ltime);
               }
            } else
               date[0] = 0;

            if (!filltext) {
               /* submit entry */
               if (el_submit
                       (lbs, 0, FALSE, date, attr_list,
                        (char (*)[NAME_LENGTH]) (list + attr_offset * NAME_LENGTH), n_attr, "", "", "",
                        "plain",
                        NULL, TRUE, NULL, NULL))
                  n_imported++;
            } else {
               strlcpy(line, list + textcol * NAME_LENGTH, 10000);
               insert_breaks(line, 78, 10000);

               for (i = textcol; i < n_attr + attr_offset; i++)
                  strlcpy(list + i * NAME_LENGTH, list + (i + 1) * NAME_LENGTH, NAME_LENGTH);

               /* submit entry */
               if (el_submit
                       (lbs, 0, FALSE, date, attr_list,
                        (char (*)[NAME_LENGTH]) (list + attr_offset * NAME_LENGTH), n_attr, line, "", "",
                        "plain",
                        NULL, TRUE, NULL, NULL))
                  n_imported++;
            }
         }
      }

      first = FALSE;

   } while (*p);

   xfree(line);
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