void show_elog_list(LOGBOOK *lbs, int past_n, int last_n, int page_n, BOOL default_page, char *info) {
   int i, j, n, index, size, status, d1, m1, y1, h1, n1, c1, d2, m2, y2, h2, n2, c2, n_line, flags,
           printable, n_logbook, n_display, reverse, numeric,
           n_attr_disp, n_msg, search_all, message_id, n_page, i_start, i_stop, in_reply_to_id,
           page_mid, page_mid_head, level, refresh, disp_attr_flags[MAX_N_ATTR + 4];
   char date[80], attrib[MAX_N_ATTR][NAME_LENGTH], disp_attr[MAX_N_ATTR + 4][NAME_LENGTH], *list, *text,
           *text1, in_reply_to[80], reply_to[MAX_REPLY_TO * 10], attachment[MAX_ATTACHMENTS][MAX_PATH_LENGTH],
           encoding[80], locked_by[256], str[NAME_LENGTH], ref[256], img[80], comment[NAME_LENGTH], mode[80],
           mid[80], menu_str[1000], menu_item[MAX_N_LIST][NAME_LENGTH], param[NAME_LENGTH], format[80],
           sort_attr[MAX_N_ATTR + 4][NAME_LENGTH], mode_cookie[80], charset[25], sort_item[NAME_LENGTH],
           refr[80], str2[80], draft[256];
   char *p, *pt1, *pt2, *slist, *svalue, *gattr, line[1024], iattr[256];
   BOOL show_attachments, threaded, csv, xml, raw, mode_commands, expand, filtering, date_filtering,
           disp_filter, show_text, text_in_attr, searched, found, disp_attr_link[MAX_N_ATTR + 4],
           sort_attributes, show_att_column = 0;
   time_t ltime, ltime_start, ltime_end, now, ltime1, ltime2, entry_ltime;
   struct tm tms, *ptms;
   MSG_LIST *msg_list;
   LOGBOOK *lbs_cur;
   regex_t re_buf[MAX_N_ATTR + 1];
   regmatch_t pmatch[10];

   /* redirect if empty parameters */
   if (strstr(_cmdline, "=&")) {
      while ((pt1 = strstr(_cmdline, "=&")) != NULL) {
         pt2 = pt1;
         while (*pt1 != '&' && *pt1 != '?')
            pt1--;
         pt1++;
         strcpy(param, pt1);
         param[pt2 - pt1] = 0;
         memmove(pt1, pt2 + 2, strlen(pt2 + 2) + 1);

         /* remove param from lastcmd if present */
         if ((pt1 = strstr(_cmdline, "lastcmd=")) != NULL) {
            sprintf(str, "%s%%3D", param);
            if ((pt1 = strstr(_cmdline, str)) != NULL) {
               pt2 = pt1 + strlen(str);
               while (*pt2 && *pt2 != '%')
                  pt2++;
               if (*pt2 == '%')
                  pt2 += 3;
               memmove(pt1, pt2, strlen(pt2) + 1);
            }
         }
      }
      if (_cmdline[strlen(_cmdline) - 1] == '=') {
         pt1 = _cmdline + strlen(_cmdline) - 1;
         while (*pt1 != '&' && *pt1 != '?')
            pt1--;
         pt1++;
         strcpy(param, pt1);
         if (param[strlen(param) - 1] == '=')
            param[strlen(param) - 1] = 0;
         *pt1 = 0;

         /* remove param from lastcmd if present */
         if ((pt1 = strstr(_cmdline, "lastcmd=")) != NULL) {
            sprintf(str, "%s%%3D", param);
            if ((pt1 = strstr(_cmdline, str)) != NULL) {
               pt2 = pt1 + strlen(str);
               while (*pt2 && *pt2 != '%' && *pt2 != '&')
                  pt2++;
               if (*pt2 == '%')
                  pt2 += 3;
               memmove(pt1, pt2, strlen(pt2) + 1);
            }
         }
      }
      if (_cmdline[strlen(_cmdline) - 1] == '&')
         _cmdline[strlen(_cmdline) - 1] = 0;
      redirect(lbs, _cmdline);
      return;
   }

   /* redirect "go" command */
   if (isparam("lastcmd")) {

      strlcpy(str, getparam("lastcmd"), sizeof(str));
      url_decode(str);

      /* subsitute "last" in command line from new parameter */
      if (isparam("last")) {
         if (strieq(getparam("last"), "_all_"))
            subst_param(str, sizeof(str), "last", "");
         else
            subst_param(str, sizeof(str), "last", getparam("last"));
      }

      /* subsitute attributes in command line from new parameter */
      for (i = 0; i < MAX_N_ATTR; i++)
         if (isparam(attr_list[i])) {
            if (strieq(getparam(attr_list[i]), "_all_"))
               subst_param(str, sizeof(str), attr_list[i], "");
            else
               subst_param(str, sizeof(str), attr_list[i], getparam(attr_list[i]));
         }

      /* do the same for subtext */
      if (isparam("subtext"))
         subst_param(str, sizeof(str), "subtext", getparam("subtext"));

      redirect(lbs, str);
      return;
   }

   /* remove remaining "_all_" in parameters */
   if (isparam("last") && strieq(getparam("last"), "_all_")) {
      strlcpy(str, _cmdline, sizeof(str));
      subst_param(str, sizeof(str), "last", "");
      redirect(lbs, str);
      return;
   }

   /* remove remaining "_all_" or empty or "--+<attrib>+--" parameters */
   strlcpy(str, _cmdline, sizeof(str));
   found = 0;
   for (i = 0; i < MAX_N_ATTR; i++) {
      if (isparam(attr_list[i])) {
         if (strieq(getparam(attr_list[i]), "_all_")) {
            subst_param(str, sizeof(str), attr_list[i], "");
            found = 1;
         }
         if (*getparam(attr_list[i]) == 0) {
            subst_param(str, sizeof(str), attr_list[i], "");
            found = 1;
         }
         sprintf(ref, "-- %s --", attr_list[i]);
         if (strieq(getparam(attr_list[i]), ref)) {
            subst_param(str, sizeof(str), attr_list[i], "");
            found = 1;
         }
      }
   }

   if (isparam("subtext")) {
      if (*getparam("subtext") == 0) {
         subst_param(str, sizeof(str), "subtext", "");
         found = 1;
      }
      sprintf(ref, "-- %s --", loc("Text"));
      if (strieq(getparam("subtext"), ref)) {
         subst_param(str, sizeof(str), "subtext", "");
         found = 1;
      }
   }

   if (found) {
      redirect(lbs, str);
      return;
   }

   slist = (char *) xmalloc((MAX_N_ATTR + 10) * NAME_LENGTH);
   svalue = (char *) xmalloc((MAX_N_ATTR + 10) * NAME_LENGTH);
   gattr = (char *) xmalloc(MAX_N_ATTR * NAME_LENGTH);
   list = (char *) xmalloc(10000);

   printable = isparam("Printable") ? atoi(getparam("Printable")) : 0;

   /* in printable mode, display all pages */
   if (printable)
      page_n = -1;

   if (isparam("Reverse"))
      reverse = atoi(getparam("Reverse"));
   else {
      reverse = 0;
      if (getcfg(lbs->name, "Reverse sort", str, sizeof(str)))
         reverse = atoi(str);
   }

   /* get message ID from "list" command */
   if (isparam("id"))
      page_mid = atoi(getparam("id"));
   else
      page_mid = 0;
   page_mid_head = 0;

   /* default mode */
   strlcpy(mode, "Summary", sizeof(mode));
   show_attachments = FALSE;

   /* check for valid page_n */
   if (page_n < -1)
      page_n = 0;

   if (past_n || last_n || page_n || page_mid || default_page) {
      /* for page display, get mode from config file */
      if (getcfg(lbs->name, "Display Mode", str, sizeof(str)))
         strlcpy(mode, str, sizeof(mode));

      /* supersede mode from cookie */
      if (isparam("elmode"))
         strlcpy(mode, getparam("elmode"), sizeof(mode));

      /* supersede mode from direct parameter */
      if (isparam("mode"))
         strlcpy(mode, getparam("mode"), sizeof(mode));

   } else {
      /* for find result, get mode from find form */
      if (isparam("mode"))
         strlcpy(mode, getparam("mode"), sizeof(mode));
      else
         strlcpy(mode, "Full", sizeof(mode));
   }

   /* set cookie if mode changed */
   mode_cookie[0] = 0;
   if (strieq(mode, "Summary") || strieq(mode, "Full") || strieq(mode, "Threaded")) {
      if (!isparam("elmode") || !strieq(getparam("elmode"), mode))
         sprintf(mode_cookie, "elmode=%s", mode);
   }

   threaded = strieq(mode, "threaded");
   csv = strieq(mode, "CSV1") || strieq(mode, "CSV2") || strieq(mode, "CSV3");
   xml = strieq(mode, "XML");
   raw = strieq(mode, "Raw");

   if (csv || xml || raw) {
      page_n = -1;              /* display all pages */
      show_attachments = FALSE; /* hide attachments */
   }

   /* show attachments in full mode by default */
   if (strieq(mode, "Full"))
      show_attachments = TRUE;

   /* supersede attachment mode if in cookie */
   if (isparam("elattach"))
      show_attachments = atoi(getparam("elattach"));

   /* supersede attachment mode if in parameter */
   if (isparam("attach"))
      show_attachments = atoi(getparam("attach"));

   /* set cookie if attachment mode changed in full view */
   if (mode_cookie[0] == 0 && strieq(mode, "Full")) {
      if (!isparam("elattach") || atoi(getparam("elattach")) != show_attachments)
         sprintf(mode_cookie, "elattach=%d", show_attachments);
   }

   /*---- convert dates to ltime ----*/

   time(&now);
   ptms = localtime(&now);
   assert(ptms);

   ltime_end = ltime_start = 0;

   d1 = m1 = y1 = h1 = n1 = c1 = d2 = m2 = y2 = h2 = n2 = c2 = 0;

   if (!past_n && !last_n) {

      ltime_start = retrieve_date("a", TRUE);
      if (ltime_start < 0) {
         xfree(slist);
         xfree(svalue);
         xfree(gattr);
         xfree(list);
         return;
      }

      if (ltime_start) {
         memcpy(&tms, localtime(&ltime_start), sizeof(struct tm));
         y1 = tms.tm_year + 1900;
         m1 = tms.tm_mon + 1;
         d1 = tms.tm_mday;
         h1 = tms.tm_hour;
         n1 = tms.tm_min;
         c1 = tms.tm_sec;
      }

      ltime_end = retrieve_date("b", FALSE);
      if (ltime_end < 0) {
         xfree(slist);
         xfree(svalue);
         xfree(gattr);
         xfree(list);
         return;
      }

      if (ltime_end) {

         if (ltime_end <= ltime_start) {
            sprintf(str, "Error: Start date after end date");
            show_error(str);
            xfree(slist);
            xfree(svalue);
            xfree(gattr);
            xfree(list);
            return;
         }

         memcpy(&tms, localtime(&ltime_end), sizeof(struct tm));
         y2 = tms.tm_year + 1900;
         m2 = tms.tm_mon + 1;
         d2 = tms.tm_mday;
         h2 = tms.tm_hour;
         n2 = tms.tm_min;
         c2 = tms.tm_sec;
      }
   }

   if (ltime_start && ltime_end && ltime_start > ltime_end) {
      show_error(loc("Error: start date after end date"));
      xfree(slist);
      xfree(svalue);
      xfree(gattr);
      xfree(list);
      return;
   }

   /*---- if user present but not allowed, log it out (required when several logbooks are
          used with different access rights and global passwords ----*/
   if (isparam("unm") && !check_login_user(lbs, getparam("unm"))) {
      unsetparam("unm");
      sid_remove(getparam("sid"));
      set_sid_cookie(lbs, "", "");
   }

   /*---- apply last login cut ----*/

   if (isparam("new_entries") && atoi(getparam("new_entries")) == 1 && isparam("unm"))
      get_user_line(lbs, getparam("unm"), NULL, NULL, NULL, NULL, &ltime_start, NULL);

   /*---- assemble message list ----*/

   /* check for search all */
   search_all = isparam("all") ? atoi(getparam("all")) : 0;

   if (getcfg(lbs->name, "Search all logbooks", str, sizeof(str)) && atoi(str) == 0)
      search_all = 0;

   n_msg = 0;
   n_display = 0;
   if (search_all) {
      /* count logbooks */
      for (n_logbook = 0;; n_logbook++) {
         if (!lb_list[n_logbook].name[0])
            break;

         if (lbs->top_group[0] && !strieq(lbs->top_group, lb_list[n_logbook].top_group))
            continue;

         if (isparam("unm") && !check_login_user(&lb_list[n_logbook], getparam("unm")))
            continue;

         n_msg += *lb_list[n_logbook].n_el_index;
      }
   } else {
      n_logbook = 1;
      n_msg = *lbs->n_el_index;
   }

   msg_list = (MSG_LIST *) xmalloc(sizeof(MSG_LIST) * n_msg);

   lbs_cur = lbs;
   numeric = TRUE;
   for (i = n = 0; i < n_logbook; i++) {
      if (search_all)
         lbs_cur = &lb_list[i];

      if (lbs->top_group[0] && !strieq(lbs->top_group, lbs_cur->top_group))
         continue;

      if (isparam("unm") && !check_login_user(lbs_cur, getparam("unm")))
         continue;

      for (j = 0; j < *lbs_cur->n_el_index; j++) {
         msg_list[n].lbs = lbs_cur;
         msg_list[n].index = j;
         msg_list[n].number = (int) lbs_cur->el_index[j].file_time;
         msg_list[n].in_reply_to = lbs_cur->el_index[j].in_reply_to;
         n++;
      }
   }

   /*---- apply start/end date cut ----*/

   date_filtering = FALSE;

   if (past_n > 0)
      ltime_start = now - 3600 * 24 * past_n;   // past n days
   else if (past_n < 0)
      ltime_start = now + 3600 * past_n;        // past n hours

   if (last_n && last_n < n_msg) {
      date_filtering = TRUE;
      for (i = n_msg - last_n - 1; i >= 0; i--)
         msg_list[i].lbs = NULL;
   }

   if (ltime_start) {
      date_filtering = TRUE;
      for (i = 0; i < n_msg; i++)
         if (msg_list[i].lbs && msg_list[i].lbs->el_index[msg_list[i].index].file_time < ltime_start)
            msg_list[i].lbs = NULL;
   }

   if (ltime_end) {
      date_filtering = TRUE;
      for (i = 0; i < n_msg; i++)
         if (msg_list[i].lbs && msg_list[i].lbs->el_index[msg_list[i].index].file_time > ltime_end)
            msg_list[i].lbs = NULL;
   }

   if (isparam("last") || getcfg(lbs->name, "Last default", str, sizeof(str))) {
      date_filtering = TRUE;
      if (isparam("last"))
         n = atoi(getparam("last"));
      else
         n = atoi(str);

      if (n > 0) {
         for (i = 0; i < n_msg; i++)
            if (msg_list[i].lbs && msg_list[i].lbs->el_index[msg_list[i].index].file_time < now - 3600 * 24
                                                                                                  * n)
               msg_list[i].lbs = NULL;
      }
   }

   /*---- filter message list ----*/

   filtering = FALSE;
   show_text = TRUE;
   searched = found = FALSE;

   for (i = 0; i < lbs->n_attr; i++) {
      /* check if attribute filter */
      if (isparam(attr_list[i]))
         break;

      if (attr_flags[i] & (AF_DATE | AF_DATETIME)) {
         sprintf(str, "%da", i);
         if (retrieve_date(str, TRUE))
            break;
         sprintf(str, "%db", i);
         if (retrieve_date(str, TRUE))
            break;
      }

      if (attr_flags[i] & AF_MULTI) {
         for (j = 0; j < MAX_N_LIST && attr_options[i][j][0]; j++) {
            sprintf(str, "%s_%d", attr_list[i], j);
            if (isparam(str)) {
               filtering = TRUE;
               break;
            }
         }
      }

      if (attr_flags[i] & (AF_MUSERLIST | AF_MUSEREMAIL)) {
         for (j = 0; j < MAX_N_LIST; j++) {
            sprintf(str, "%s_%d", attr_list[i], j);
            if (isparam(str)) {
               filtering = TRUE;
               break;
            }
         }
      }

      /* check if sort by attribute */
      if ((isparam("sort") && strieq(getparam("sort"), attr_list[i]))
          || (isparam("rsort") && strieq(getparam("rsort"), attr_list[i])))
         break;
   }

   /* turn on filtering if found */
   if (i < lbs->n_attr)
      filtering = TRUE;

   if (isparam("subtext"))
      filtering = TRUE;

   if (getcfg(lbs->name, "Sort Attributes", list, 10000))
      filtering = TRUE;

   text = (char *) xmalloc(TEXT_SIZE);
   text1 = (char *) xmalloc(TEXT_SIZE);

   /* prepare for regex search */
   memset(re_buf, 0, sizeof(re_buf));

   /* compile regex for subtext */
   if (isparam("subtext")) {
      strlcpy(str, getparam("subtext"), sizeof(str));
      flags = REG_EXTENDED;
      if (!isparam("casesensitive"))
         flags |= REG_ICASE;
      status = regcomp(re_buf, str, flags);
      if (status) {
         sprintf(line, loc("Error in regular expression \"%s\""), str);
         strlcat(line, ": ", sizeof(line));
         regerror(status, re_buf, str, sizeof(str));
         strlcat(line, str, sizeof(line));
         strencode2(str, line, sizeof(str));
         show_error(str);
         return;
      }
   }

   /* compile regex for attributes */
   for (i = 0; i < lbs->n_attr; i++) {
      if (isparam(attr_list[i])) {
         strlcpy(str, getparam(attr_list[i]), sizeof(str));

         /* if value starts with '$', substitute it */
         if (str[0] == '$') {
            j = build_subst_list(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, attrib,
                                 TRUE);
            add_subst_time(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, "entry time",
                           date, &j, 0);

            strsubst_list(str, sizeof(str), (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, j);
            setparam(attr_list[i], str);
         }

         flags = REG_EXTENDED;

         if (!isparam("casesensitive"))
            flags |= REG_ICASE;

         status = regcomp(re_buf + i + 1, str, flags);
         if (status) {
            sprintf(line, loc("Error in regular expression \"%s\""), str);
            strlcat(line, ": ", sizeof(line));
            regerror(status, re_buf + i + 1, str, sizeof(str));
            strlcat(line, str, sizeof(line));
            strencode2(str, line, sizeof(str));
            show_error(str);
            return;
         }
      }
   }

   sort_item[0] = 0;
   if (isparam("sort"))
      strlcpy(sort_item, getparam("sort"), sizeof(sort_item));
   if (isparam("rsort"))
      strlcpy(sort_item, getparam("rsort"), sizeof(sort_item));

   sort_attributes = getcfg(lbs->name, "Sort Attributes", str, sizeof(str));

   /* do filtering */
   for (index = 0; index < n_msg; index++) {
      if (!msg_list[index].lbs)
         continue;

      /* retrieve message */
      size = TEXT_SIZE;
      message_id = msg_list[index].lbs->el_index[msg_list[index].index].message_id;

      if (filtering) {
         status = el_retrieve(msg_list[index].lbs, message_id, date, attr_list, attrib, lbs->n_attr, text,
                              &size, in_reply_to, reply_to, attachment, encoding, locked_by, draft);
         if (status != EL_SUCCESS)
            break;

         /* apply filter for attributes */
         for (i = 0; i < lbs->n_attr; i++) {

            /* replace icon name with their comments if present */
            if (attr_flags[i] & AF_ICON) {
               sprintf(str, "Icon comment %s", attrib[i]);
               if (getcfg(lbs->name, str, comment, sizeof(comment)))
                  strlcpy(attrib[i], comment, NAME_LENGTH);
            }

            /* check for multi attributes */
            if (attr_flags[i] & AF_MULTI) {

               /* OR of any of the values */
               searched = found = FALSE;
               for (j = 0; j < MAX_N_LIST && attr_options[i][j][0]; j++) {
                  sprintf(str, "%s_%d", attr_list[i], j);
                  if (isparam(str)) {
                     searched = TRUE;
                     if (strstr(attrib[i], getparam(str))) {
                        found = TRUE;
                        break;
                     }
                  }
               }

               /* search for parameter without '_' coming from quick filter */
               if (isparam(attr_list[i])) {
                  searched = TRUE;

                  strlcpy(str, getparam(attr_list[i]), sizeof(str));
                  if (str[0] == '^' && str[strlen(str) - 1] == '$') {
                     str[strlen(str) - 1] = 0;
                     strlcpy(comment, str + 1, NAME_LENGTH);
                  } else
                     strlcpy(comment, str, NAME_LENGTH);
                  strlcpy(str, comment, sizeof(str));

                  if (strstr(attrib[i], str))
                     found = TRUE;
               }

               if (searched && !found)
                  break;
            }

               /* check for multi user list or multi user email */
            else if (attr_flags[i] & (AF_MUSERLIST | AF_MUSEREMAIL)) {

               /* OR of any of the values */
               searched = found = FALSE;
               for (j = 0; j < MAX_N_LIST; j++) {
                  sprintf(str, "%s_%d", attr_list[i], j);
                  if (isparam(str)) {
                     searched = TRUE;
                     if (strstr(attrib[i], getparam(str))) {
                        found = TRUE;
                        break;
                     }
                  }
               }

               /* search for parameter without '_' coming from quick filter */
               if (isparam(attr_list[i])) {
                  searched = TRUE;
                  if (strstr(attrib[i], getparam(attr_list[i])))
                     found = TRUE;
               }

               if (searched && !found)
                  break;

            } else if (attr_flags[i] & (AF_DATE | AF_DATETIME)) {

               /* check for last[i]/next[i] */

               ltime = isparam(attr_list[i]) ? atoi(getparam(attr_list[i])) : 0;

               /* today 12h noon */
               time(&now);
               memcpy(&tms, localtime(&now), sizeof(struct tm));
               tms.tm_hour = 12;
               tms.tm_min = 0;
               tms.tm_sec = 0;
               now = mktime(&tms);

               /* negative i: last [i] days */
               if (ltime < 0)
                  if (atoi(attrib[i]) < now + ltime * 3600 * 24 - 3600 * 12 || atoi(attrib[i]) > now)
                     break;

               /* positive i: next [i] days */
               if (ltime > 0)
                  if (atoi(attrib[i]) > now + ltime * 3600 * 24 + 3600 * 12 || atoi(attrib[i]) < now)
                     break;

               /* check for start date / end date */
               sprintf(str, "%da", i);
               ltime = retrieve_date(str, TRUE);
               if (ltime > 0 && atoi(attrib[i]) < ltime)
                  break;

               sprintf(str, "%db", i);
               ltime = retrieve_date(str, FALSE);
               if (ltime > 0 && (atoi(attrib[i]) > ltime || atoi(attrib[i]) == 0))
                  break;

            } else {

               strlcpy(str, isparam(attr_list[i]) ? getparam(attr_list[i]) : "", sizeof(str));

               /* if value starts with '$', substitute it */
               if (str[0] == '$') {
                  j = build_subst_list(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue,
                                       attrib, TRUE);
                  sprintf(mid, "%d", message_id);
                  add_subst_list((char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, "message id",
                                 mid, &j);
                  add_subst_time(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue,
                                 "entry time", date, &j, 0);

                  strsubst_list(str, sizeof(str), (char (*)[NAME_LENGTH]) slist,
                                (char (*)[NAME_LENGTH]) svalue, j);
                  setparam(attr_list[i], str);
               }

               if (isparam(attr_list[i])) {
                  status = regexec(re_buf + 1 + i, attrib[i], 10, pmatch, 0);
                  if (status == REG_NOMATCH)
                     break;
               }
            }
         }

         if (i < lbs->n_attr) {
            msg_list[index].lbs = NULL;
            continue;
         }

         if (isparam("subtext")) {

            status = regexec(re_buf, text, 10, pmatch, 0);
            if (isparam("sall") && atoi(getparam("sall")) && status == REG_NOMATCH) {

               // search text in attributes
               for (i = 0; i < lbs->n_attr; i++) {
                  status = regexec(re_buf, attrib[i], 10, pmatch, 0);
                  if (status != REG_NOMATCH)
                     break;
               }

               if (i == lbs->n_attr) {
                  msg_list[index].lbs = NULL;
                  continue;
               }
            } else if (status == REG_NOMATCH) {
               msg_list[index].lbs = NULL;
               continue;
            }
         }
      }                         // if (filtering)

      /* evaluate "sort attributes" */
      if (sort_attributes) {
         getcfg(lbs->name, "Sort Attributes", list, 10000);
         msg_list[index].string[0] = 0;
         n = strbreak(list, sort_attr, MAX_N_ATTR, ",", FALSE);
         for (i = 0; i < n; i++) {
            for (j = 0; j < lbs->n_attr; j++) {
               if (strieq(sort_attr[i], attr_list[j])) {
                  strlcat(msg_list[index].string, " ", sizeof(msg_list[index].string));
                  strlcat(msg_list[index].string, attrib[j], sizeof(msg_list[index].string));
                  if (attr_flags[i] & (AF_NUMERIC | AF_DATETIME | AF_DATE)) {
                     msg_list[index].number = atoi(attrib[j]);
                     numeric = TRUE;
                  } else
                     numeric = FALSE;
                  break;
               }
            }
            if (strieq(sort_attr[i], loc("ID"))) {
               strlcat(msg_list[index].string, " ", sizeof(msg_list[index].string));
               sprintf(str, "%08d", message_id);
               strlcat(msg_list[index].string, str, sizeof(msg_list[index].string));
            } else if (strieq(sort_attr[i], loc("Logbook"))) {
               strlcat(msg_list[index].string, " ", sizeof(msg_list[index].string));
               strlcat(msg_list[index].string, msg_list[index].lbs->name, sizeof(msg_list[index].string));
            } else if (strieq(sort_attr[i], loc("Date"))) {
               strlcat(msg_list[index].string, " ", sizeof(msg_list[index].string));
               entry_ltime = date_to_ltime(date);
               sprintf(str, "%08d", (int) entry_ltime);
               strlcat(msg_list[index].string, str, sizeof(msg_list[index].string));
            }
         }
      }

      /* add attribute for sorting */
      if (sort_item[0]) {
         for (i = 0; i < lbs->n_attr; i++) {
            if (strieq(sort_item, attr_list[i])) {
               if (attr_flags[i] & (AF_NUMERIC | AF_DATETIME | AF_DATE)) {
                  numeric = TRUE;
                  msg_list[index].number = atoi(attrib[i]);
               } else {
                  numeric = FALSE;
                  strlcpy(msg_list[index].string, attrib[i], 256);
               }
            }

            if (strieq(sort_item, loc("ID"))) {
               numeric = TRUE;
               msg_list[index].number = message_id;
            }

            if (strieq(sort_item, loc("Logbook")))
               strlcpy(msg_list[index].string, msg_list[index].lbs->name, 256);
         }

         if (isparam("rsort"))
            reverse = 1;

         if (isparam("sort"))
            reverse = 0;
      }
   }

   /*---- in threaded mode, set date of latest entry of thread ----*/

   if (threaded && !filtering && !date_filtering) {
      for (index = 0; index < n_msg; index++) {
         if (!msg_list[index].lbs)
            continue;

         message_id = msg_list[index].lbs->el_index[msg_list[index].index].message_id;
         in_reply_to_id = msg_list[index].lbs->el_index[msg_list[index].index].in_reply_to;
         if (!in_reply_to_id)
            continue;

         do {
            message_id = in_reply_to_id;

            /* search index of message */
            for (i = 0; i < *msg_list[index].lbs->n_el_index; i++)
               if (msg_list[index].lbs->el_index[i].message_id == message_id)
                  break;

            /* stop if not found */
            if (i == *msg_list[index].lbs->n_el_index)
               break;

            in_reply_to_id = msg_list[index].lbs->el_index[i].in_reply_to;

         } while (in_reply_to_id);

         /* if head not found, skip message */
         if (i == *msg_list[index].lbs->n_el_index) {
            msg_list[index].lbs = NULL;
            continue;
         }

         /* set new page message ID with head message */
         if (page_mid && msg_list[index].lbs->el_index[msg_list[index].index].message_id == page_mid)
            page_mid_head = message_id;

         /* search message head in list */
         for (j = 0; j < n_msg; j++)
            if (msg_list[j].lbs == msg_list[index].lbs && msg_list[j].index == i)
               break;

         if (j < index) {
            /* set date from current message, if later */
            if (msg_list[j].number < msg_list[index].number)
               msg_list[j].number = msg_list[index].number;
         }

         /* now delete current message, to leave only heads in list */
         msg_list[index].lbs = NULL;
      }
   }

   /*---- compact messasges ----*/

   for (i = j = 0; i < n_msg; i++)
      if (msg_list[i].lbs)
         memcpy(&msg_list[j++], &msg_list[i], sizeof(MSG_LIST));
   n_msg = j;

   /*---- sort messasges ----*/

   if (numeric)
      qsort(msg_list, n_msg, sizeof(MSG_LIST), reverse ? msg_compare_reverse_numeric : msg_compare_numeric);
   else
      qsort(msg_list, n_msg, sizeof(MSG_LIST), reverse ? msg_compare_reverse : msg_compare);

   /*---- search page for specific message ----*/

   if (getcfg(lbs->name, "Entries per page", str, sizeof(str)))
      n_page = atoi(str);
   else
      n_page = 20;
   if (isparam("npp")) {
      n_page = atoi(getparam("npp"));
      if (n_page < 1)
         n_page = 1;
      if (n_page > 100000)
         n_page = 100000;
   }

   if (page_mid) {
      default_page = 0;

      for (i = 0; i < n_msg; i++)
         if (msg_list[i].lbs->el_index[msg_list[i].index].message_id == page_mid
             || msg_list[i].lbs->el_index[msg_list[i].index].message_id == page_mid_head)
            break;

      if (i < n_msg)
         page_n = i / n_page + 1;
   }

   /*---- number of messages per page ----*/

   n_attr_disp = n_line = 0;
   i_start = 0;
   i_stop = n_msg - 1;
   if (!csv && !xml && !raw) {
      if (page_n || default_page) {
         if (default_page && page_n != -1)
            page_n = reverse ? 1 : (n_msg - 1) / n_page + 1;

         if (page_n != -1) {
            i_start = (page_n - 1) * n_page;
            i_stop = i_start + n_page - 1;

            if (i_start >= n_msg && n_msg > 0) {
               page_n = 1;
               i_start = 0;
            }

            if (i_stop >= n_msg)
               i_stop = n_msg - 1;
         }
      }
   }

   /*---- header ----*/

   if (getcfg(lbs->name, "List Page Title", str, sizeof(str))) {
      i = build_subst_list(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, NULL, TRUE);
      strsubst_list(str, sizeof(str), (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, i);
      strip_html(str);
   } else
      sprintf(str, "ELOG %s", lbs->name);

   if (csv) {

      /* no menus and tables */
      show_plain_header(0, "export.csv");

      rsprintf("\"%s\"", loc("Message ID"));
      if (strieq(mode, "CSV1"))
         rsprintf(",");
      else
         rsprintf(";");

      rsprintf("\"%s\"", loc("Date"));
      if (strieq(mode, "CSV1"))
         rsprintf(",");
      else
         rsprintf(";");

      for (i = 0; i < lbs->n_attr; i++) {
         strlcpy(str, attr_list[i], sizeof(str));
         if (str[0]) {
            rsputs("\"");
            pt1 = str;
            while ((pt2 = strchr(pt1, '"')) != NULL) {
               *pt2 = 0;
               rsputs(pt1);
               rsputs("\"\"");
               pt1 = pt2 + 1;
            }
            rsputs(pt1);
            rsputs("\"");
         }
         if (i < lbs->n_attr - 1) {
            if (strieq(mode, "CSV1"))
               rsprintf(",");
            else
               rsprintf(";");
         } else {
            if (strieq(mode, "CSV3"))
               rsprintf(";\"Text\"");
            rsprintf("\r\n");
         }
      }

   } else if (xml) {

      /* no menus and tables */
      show_plain_header(0, "export.xml");
      if (!getcfg("global", "charset", charset, sizeof(charset)))
         strcpy(charset, DEFAULT_HTTP_CHARSET);
      rsprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n", charset);
      rsprintf("<!-- ELOGD Version %s export.xml -->\n", VERSION);
      rsprintf("<ELOG_LIST>\n");

   } else if (raw) {

      /* no menus and tables */
      show_plain_header(0, "export.txt");

   } else {

      if (getcfg(lbs->name, "Refresh", refr, sizeof(refr)))
         refresh = atoi(refr);
      else
         refresh = 0;
      show_standard_header(lbs, TRUE, str, NULL, TRUE, mode_cookie, NULL, refresh);

      /*---- title ----*/

      strlcpy(str, ", ", sizeof(str));
      if (past_n == 1)
         strcat(str, loc("Last day"));
      else if (past_n > 1)
         sprintf(str + strlen(str), loc("Last %d days"), past_n);
      else if (past_n < 0)
         sprintf(str + strlen(str), loc("Last %d hours"), -past_n);
      else if (last_n)
         sprintf(str + strlen(str), loc("Last %d entries"), last_n);
      else if (page_n == -1)
         strlcpy(str + strlen(str), loc("all entries"), sizeof(str) - strlen(str));
      else if (page_n)
         sprintf(str + strlen(str), loc("Page %d of %d"), page_n, (n_msg - 1) / n_page + 1);
      if (strlen(str) == 2)
         str[0] = 0;

      if (printable)
         show_standard_title(lbs, str, 1);
      else
         show_standard_title(lbs, str, 0);

      /*---- menu buttons ----*/

      if (!printable) {
         rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");

         /* current command line for select command */
         strlcpy(str, isparam("cmdline") ? getparam("cmdline") : "", sizeof(str));

         /* remove select switch */
         if (strstr(str, "select=1")) {
            *strstr(str, "select=1") = 0;
            if (strlen(str) > 1 && (str[strlen(str) - 1] == '&' || str[strlen(str) - 1] == '?'))
               str[strlen(str) - 1] = 0;
         }

         /* store current command line as hidden parameter for page navigation */
         if (str[0] && !strieq(str, "?")) {
            rsprintf("<input type=hidden name=lastcmd value=\"");
            rsputs3(str);
            rsprintf("\">\n", str);
         }

         if (!getcfg(lbs->name, "Guest Find menu commands", menu_str, sizeof(menu_str)) || logged_in(lbs))
            getcfg(lbs->name, "Find menu commands", menu_str, sizeof(menu_str));

         if (!menu_str[0]) {
            if (!getcfg(lbs->name, "Guest list menu commands", menu_str, sizeof(menu_str)) || logged_in(lbs))
               getcfg(lbs->name, "list menu commands", menu_str, sizeof(menu_str));
         }

         /* default menu commands */
         if (menu_str[0] == 0) {
            strlcpy(menu_str, "New, Find, Select, Import, ", sizeof(menu_str));

            if (getcfg(lbs->name, "Password file", str, sizeof(str)))
               strlcat(menu_str, "Config, Logout, ", sizeof(menu_str));
            else
               strlcat(menu_str, "Config, ", sizeof(menu_str));

            if (getcfg(lbs->name, "Mirror server", str, sizeof(str)))
               strlcat(menu_str, "Synchronize, ", sizeof(menu_str));

            strlcpy(str, loc("Last x"), sizeof(str));
            strlcat(menu_str, "Last x, Help, ", sizeof(menu_str));
         }

         n = strbreak(menu_str, menu_item, MAX_N_LIST, ",", FALSE);

         for (i = 0; i < n; i++) {
            if (is_user_allowed(lbs, menu_item[i])) {
               if (strieq(menu_item[i], "Last x")) {
                  if (past_n > 0) {
                     sprintf(str, loc("Last %d days"), past_n * 2);
                     rsprintf("&nbsp;<a href=\"past%d?mode=%s\">%s</a>&nbsp;|\n", past_n * 2, mode, str);
                  } else {
                     strlcpy(str, loc("Last day"), sizeof(str));
                     rsprintf("&nbsp;<a href=\"past1?mode=%s\">%s</a>&nbsp;|\n", mode, str);
                  }

                  if (last_n) {
                     sprintf(str, loc("Last %d entries"), last_n * 2);
                     rsprintf("&nbsp;<a href=\"last%d?mode=%s\">%s</a>&nbsp;|\n", last_n * 2, mode, str);
                  }
               } else if (strieq(menu_item[i], "Select")) {
                  strlcpy(str, getparam("cmdline"), sizeof(str));
                  if (isparam("select") && atoi(getparam("select")) == 1) {
                     /* remove select switch */
                     if (strstr(str, "select=1")) {
                        *strstr(str, "select=1") = 0;
                        if (strlen(str) > 1 && (str[strlen(str) - 1] == '&' || str[strlen(str) - 1] == '?'))
                           str[strlen(str) - 1] = 0;
                     }
                  } else {
                     /* add select switch */
                     if (strchr(str, '?'))
                        strcat(str, "&select=1");
                     else
                        strcat(str, "?select=1");
                  }
                  rsprintf("&nbsp;<a href=\"");
                  rsputs3(str);
                  rsprintf("\">%s</a>&nbsp;|\n", loc("Select"));
               } else {
                  strlcpy(str, loc(menu_item[i]), sizeof(str));
                  url_encode(str, sizeof(str));

                  if (i < n - 1)
                     rsprintf("&nbsp;<a href=\"?cmd=%s\">%s</a>&nbsp;|\n", str, loc(menu_item[i]));
                  else
                     rsprintf("&nbsp;<a href=\"?cmd=%s\">%s</a>&nbsp;\n", str, loc(menu_item[i]));
               }
            }
         }

         rsprintf("</span></td></tr>\n\n");
      }

      /*---- list menu text ----*/

      if ((getcfg(lbs->name, "find menu text", str, sizeof(str)) || getcfg(lbs->name, "list menu text", str,
                                                                           sizeof(str))) && !printable) {
         FILE *f;
         char file_name[256], *buf;

         rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");

         /* check if file starts with an absolute directory */
         if (str[0] == DIR_SEPARATOR || str[1] == ':')
            strlcpy(file_name, str, sizeof(file_name));
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

         rsprintf("</span></td></tr>");
      }

      /*---- display filters ----*/

      disp_filter = isparam("ma") || isparam("ya") || isparam("da") || isparam("mb") || isparam("yb")
                    || isparam("db") || isparam("subtext") || isparam("last");

      for (i = 0; i < lbs->n_attr; i++)
         if (isparam(attr_list[i]) && (attr_flags[i] & (AF_DATE | AF_DATETIME)) == 0)
            disp_filter = TRUE;

      for (i = 0; i < lbs->n_attr; i++) {
         if (attr_flags[i] & (AF_DATE | AF_DATETIME)) {
            sprintf(str, "%da", i);
            ltime = retrieve_date(str, TRUE);
            if (ltime > 0)
               disp_filter = TRUE;
            sprintf(str, "%db", i);
            ltime = retrieve_date(str, FALSE);
            if (ltime > 0)
               disp_filter = TRUE;
         }
         if (attr_flags[i] & (AF_MULTI | AF_MUSERLIST | AF_MUSEREMAIL)) {
            for (j = 0; j < MAX_N_LIST; j++) {
               sprintf(str, "%s_%d", attr_list[i], j);
               if (isparam(str))
                  disp_filter = TRUE;
               if (isparam(attr_list[i]))
                  disp_filter = TRUE;
            }
         }
      }

      if (isparam("new_entries") && atoi(getparam("new_entries")) == 1) {
         rsprintf("<tr><td class=\"listframe\">\n");
         rsprintf("<table width=\"100%%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
         rsprintf("<tr><td nowrap width=\"10%%\" class=\"attribname\">%s:</td>", loc("New entries since"));
         memcpy(&tms, localtime(&ltime_start), sizeof(struct tm));
         my_strftime(str, sizeof(str), "%c", &tms);
         rsprintf("<td class=\"attribvalue\">%s</td></tr>", str);
         rsprintf("</table></td></tr>\n\n");
      }

      if (disp_filter) {
         rsprintf("<tr><td class=\"listframe\">\n");
         rsprintf("<table width=\"100%%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");

         if (isparam("last")) {
            rsprintf("<tr><td nowrap width=\"10%%\" class=\"attribname\">%s:</td>",
                     loc("Restrict search to last"));
            strencode2(str, getparam("last"), sizeof(str));
            rsprintf("<td class=\"attribvalue\">%s %s</td></tr>", str, loc("days"));
         }

         if (isparam("ma") || isparam("ya") || isparam("da") || isparam("ha") || isparam("na")
             || isparam("ca")) {
            memset(&tms, 0, sizeof(struct tm));
            tms.tm_year = y1 - 1900;
            tms.tm_mon = m1 - 1;
            tms.tm_mday = d1;
            tms.tm_hour = h1;
            tms.tm_min = n1;
            tms.tm_sec = c1;
            if (tms.tm_year < 90)
               tms.tm_year += 100;
            mktime(&tms);
            if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
               strcpy(format, DEFAULT_TIME_FORMAT);
            strftime(str, sizeof(str), format, &tms);

            rsprintf("<tr><td nowrap width=\"10%%\" class=\"attribname\">%s:</td>", loc("Start date"));
            rsprintf("<td class=\"attribvalue\">%s</td></tr>", str);
         }

         if (isparam("mb") || isparam("yb") || isparam("db") || isparam("hb") || isparam("nb")
             || isparam("cb")) {
            memset(&tms, 0, sizeof(struct tm));
            tms.tm_year = y2 - 1900;
            tms.tm_mon = m2 - 1;
            tms.tm_mday = d2;
            tms.tm_hour = h2;
            tms.tm_min = n2;
            tms.tm_sec = c2;
            if (tms.tm_year < 90)
               tms.tm_year += 100;
            ltime = mktime(&tms);
            memcpy(&tms, localtime(&ltime), sizeof(struct tm));
            if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
               strcpy(format, DEFAULT_TIME_FORMAT);
            strftime(str, sizeof(str), format, &tms);

            rsprintf("<tr><td nowrap width=\"10%%\" class=\"attribname\">%s:</td>", loc("End date"));

            rsprintf("<td class=\"attribvalue\">%s</td></tr>", str);
         }

         for (i = 0; i < lbs->n_attr; i++) {
            if (attr_flags[i] & (AF_DATE | AF_DATETIME)) {

               sprintf(str, "%da", i);
               ltime1 = retrieve_date(str, TRUE);
               sprintf(str, "%db", i);
               ltime2 = retrieve_date(str, TRUE);

               if (ltime1 > 0 || ltime2 > 0) {
                  rsprintf("<tr><td nowrap width=\"10%%\" class=\"attribname\">%s:</td>", attr_list[i]);
                  rsprintf("<td class=\"attribvalue\">");
                  if (ltime1) {
                     memcpy(&tms, localtime(&ltime1), sizeof(struct tm));
                     if (attr_flags[i] & AF_DATE)
                        strcpy(format, DEFAULT_DATE_FORMAT);
                     else
                        strcpy(format, DEFAULT_TIME_FORMAT);
                     my_strftime(str, sizeof(str), format, &tms);
                     if (ltime2 > 0)
                        rsprintf("%s %s", loc("From"), str);
                     else
                        rsprintf("%s %s", loc("After"), str);
                  }
                  if (ltime2) {
                     memcpy(&tms, localtime(&ltime2), sizeof(struct tm));
                     if (attr_flags[i] & AF_DATE)
                        strcpy(format, DEFAULT_DATE_FORMAT);
                     else
                        strcpy(format, DEFAULT_TIME_FORMAT);
                     my_strftime(str, sizeof(str), format, &tms);
                     if (ltime1 > 0)
                        rsprintf(" %s %s", loc("to"), str);
                     else
                        rsprintf("%s %s", loc("Before"), str);
                  }
                  rsprintf("</td></tr>", comment);
               }

            } else if (attr_flags[i] & AF_MULTI) {

               line[0] = 0;

               for (j = 0; j < MAX_N_LIST && attr_options[i][j][0]; j++) {
                  sprintf(iattr, "%s_%d", attr_list[i], j);
                  if (isparam(iattr)) {

                     comment[0] = 0;
                     if (attr_flags[i] & AF_ICON) {
                        sprintf(str, "Icon comment %s", getparam(iattr));
                        getcfg(lbs->name, str, comment, sizeof(comment));
                     }

                     if (line[0])
                        strlcat(line, " | ", sizeof(line));

                     if (comment[0] == 0) {
                        strlcpy(str, getparam(iattr), sizeof(str));
                        if (str[0] == '^' && str[strlen(str) - 1] == '$') {
                           str[strlen(str) - 1] = 0;
                           strlcpy(comment, str + 1, NAME_LENGTH);
                        } else
                           strlcpy(comment, str, NAME_LENGTH);
                        strlcpy(str, comment, sizeof(str));

                        strencode2(line + strlen(line), str, sizeof(line) - strlen(line));
                     } else
                        strlcat(line, comment, sizeof(line));
                  }
               }

               if (isparam(attr_list[i])) {
                  comment[0] = 0;
                  if (attr_flags[i] & AF_ICON) {
                     sprintf(str, "Icon comment %s", getparam(attr_list[i]));
                     getcfg(lbs->name, str, comment, sizeof(comment));
                  }

                  if (line[0])
                     strlcat(line, " | ", sizeof(line));

                  if (comment[0] == 0) {
                     strlcpy(str, getparam(attr_list[i]), sizeof(str));
                     if (str[0] == '^' && str[strlen(str) - 1] == '$') {
                        str[strlen(str) - 1] = 0;
                        strlcpy(comment, str + 1, NAME_LENGTH);
                     } else
                        strlcpy(comment, str, NAME_LENGTH);
                     strlcpy(str, comment, sizeof(str));
                     strencode2(line + strlen(line), str, sizeof(line) - strlen(line));
                  } else
                     strlcat(line, comment, sizeof(line));
               }

               if (line[0]) {
                  rsprintf("<tr><td nowrap width=\"10%%\" class=\"attribname\">%s:</td>", attr_list[i]);
                  rsprintf("<td class=\"attribvalue\"><span style=\"color:black;background-color:#ffff66\">");
                  rsprintf("%s</span></td></tr>", line);
               }

            } else if (attr_flags[i] & (AF_MUSERLIST | AF_MUSEREMAIL)) {

               line[0] = 0;

               for (j = 0; j < MAX_N_LIST; j++) {
                  sprintf(iattr, "%s_%d", attr_list[i], j);
                  if (isparam(iattr)) {
                     if (line[0])
                        strlcat(line, " | ", sizeof(line));
                     strlcat(line, getparam(iattr), sizeof(line));
                  }
               }

               if (isparam(attr_list[i])) {
                  if (line[0])
                     strlcat(line, " | ", sizeof(line));
                  strencode2(line + strlen(line), getparam(attr_list[i]), sizeof(line) - strlen(line));
               }

               if (line[0]) {
                  rsprintf("<tr><td nowrap width=\"10%%\" class=\"attribname\">%s:</td>", attr_list[i]);
                  rsprintf("<td class=\"attribvalue\"><span style=\"color:black;background-color:#ffff66\">");
                  rsprintf("%s</span></td></tr>", line);
               }

            } else if (isparam(attr_list[i])) {

               strlcpy(str, getparam(attr_list[i]), sizeof(str));
               if (str[0] && !strieq(str, "_all_") && strncmp(str, "--", 2) != 0) {
                  comment[0] = 0;
                  if (attr_flags[i] & AF_ICON) {
                     sprintf(str, "Icon comment %s", getparam(attr_list[i]));
                     getcfg(lbs->name, str, comment, sizeof(comment));
                  }

                  if (comment[0] == 0) {
                     strlcpy(str, getparam(attr_list[i]), sizeof(str));
                     if (str[0] == '^' && str[strlen(str) - 1] == '$') {
                        str[strlen(str) - 1] = 0;
                        strlcpy(comment, str + 1, NAME_LENGTH);
                     } else
                        strlcpy(comment, str, NAME_LENGTH);
                     strlcpy(str, comment, sizeof(str));
                     strencode2(comment, str, sizeof(comment));
                  }

                  rsprintf("<tr><td nowrap width=\"10%%\" class=\"attribname\">%s:</td>", attr_list[i]);
                  rsprintf("<td class=\"attribvalue\"><span style=\"color:black;background-color:#ffff66\">");
                  rsprintf("%s</span></td></tr>", comment);
               }
            }
         }

         if (isparam("subtext")) {
            rsprintf("<tr><td nowrap width=\"10%%\" class=\"attribname\">%s:</td>", loc("Text"));
            rsprintf("<td class=\"attribvalue\"><span style=\"color:black;background-color:#ffff66\">");
            strencode2(str, getparam("subtext"), sizeof(str));
            rsprintf("%s</span></td></tr>", str);
         }

         rsprintf("</table></td></tr>\n\n");
      }

      /* get number of summary lines */
      n_line = 3;
      if (getcfg(lbs->name, "Summary lines", str, sizeof(str)))
         n_line = atoi(str);

      /* suppress summary completely if text body is disabled */
      if (getcfg(lbs->name, "Show text", str, sizeof(str)) && atoi(str) == 0)
         n_line = 0;

      /* suppress attachment colum if switched off */
      show_att_column = strieq(mode, "Summary");
      if (getcfg(lbs->name, "Enable attachments", str, sizeof(str)) && atoi(str) == 0)
         show_att_column = FALSE;

      /* get mode commands flag */
      mode_commands = TRUE;
      if (getcfg(lbs->name, "Mode commands", str, sizeof(str)) && atoi(str) == 0)
         mode_commands = FALSE;

      /*---- evaluate conditions for quick filters */
      for (i = 0; i < lbs->n_attr; i++) {
         attrib[i][0] = 0;
         if (isparam(attr_list[i])) {
            strlcpy(str, getparam(attr_list[i]), sizeof(str));
            if (str[0] == '^' && str[strlen(str) - 1] == '$') {
               str[strlen(str) - 1] = 0;
               strlcpy(attrib[i], str + 1, NAME_LENGTH);
            } else
               strlcpy(attrib[i], str, NAME_LENGTH);
         }
      }
      evaluate_conditions(lbs, attrib);

      /*---- notification message ----*/

      if (info && info[0]) {
         rsprintf("<tr><td class=\"notifyleft\">%s</td></tr>\n", info);
      }

      /*---- page navigation ----*/

      if (!printable) {
         show_page_filters(lbs, n_msg, page_n, mode_commands, mode);
         show_page_navigation(lbs, n_msg, page_n, n_page);
      }

      /*---- select navigation ----*/

      if (isparam("select") && atoi(getparam("select")) == 1)
         show_select_navigation(lbs);

      /*---- table titles ----*/

      /* overall listing table */
      rsprintf("<tr><td><table class=\"listframe\" width=\"100%%\" cellspacing=0>\n");

      size = printable ? 2 : 3;
      show_text = TRUE;
      text_in_attr = FALSE;

      list[0] = 0;
      getcfg(lbs->name, "List display", list, 10000);

      /* evaluate Guest display list */
      if (getcfg(lbs->name, "Password file", str, sizeof(str)) && getcfg(lbs->name, "Guest list display",
                                                                         str, sizeof(str))
          && !isparam("unm")) {

         strcpy(list, str);

         n = strbreak(list, (char (*)[NAME_LENGTH]) gattr, MAX_N_ATTR, ",", FALSE);
         for (j = 0; j < n; j++)
            if (strieq(gattr + j * NAME_LENGTH, "text"))
               break;

         if (n > 0 && j == n)
            show_text = FALSE;
         else
            text_in_attr = TRUE;
      }

      memset(disp_attr_flags, 0, sizeof(disp_attr_flags));
      if (list[0]) {
         n_attr_disp = strbreak(list, disp_attr, MAX_N_ATTR, ",", FALSE);

         /* if text is in guest display list, adjust number of *real* attributes */
         if (text_in_attr)
            n_attr_disp--;

         if (search_all) {
            for (i = n_attr_disp - 1; i >= 0; i--)
               strcpy(disp_attr[i + 1], disp_attr[i]);
            strcpy(disp_attr[0], loc("Logbook"));
            n_attr_disp++;
         }
      } else {
         if (search_all) {
            n_attr_disp = lbs->n_attr + 3;

            strcpy(disp_attr[0], loc("Logbook"));
            strcpy(disp_attr[1], loc("ID"));
            strcpy(disp_attr[2], loc("Date"));
            memcpy(disp_attr + 3, attr_list, sizeof(attr_list));
            memcpy(disp_attr_flags + 3, attr_flags, sizeof(attr_flags));
         } else {
            n_attr_disp = lbs->n_attr + 2;

            strcpy(disp_attr[0], loc("ID"));
            strcpy(disp_attr[1], loc("Date"));
            memcpy(disp_attr + 2, attr_list, sizeof(attr_list));
            memcpy(disp_attr_flags + 2, attr_flags, sizeof(attr_flags));
         }
      }

      list[0] = 0;
      getcfg(lbs->name, "Link display", list, 10000);
      if (list[0]) {
         n = strbreak(list, (char (*)[NAME_LENGTH]) gattr, MAX_N_ATTR, ",", FALSE);

         for (i = 0; i < n_attr_disp; i++) {
            for (j = 0; j < n; j++)
               if (strieq(gattr + j * NAME_LENGTH, disp_attr[i]))
                  break;

            if (j < n)
               disp_attr_link[i] = TRUE;
            else
               disp_attr_link[i] = FALSE;
         }
      } else
         for (i = 0; i < n_attr_disp; i++)
            disp_attr_link[i] = TRUE;

      if (threaded) {
      } else {
         rsprintf("<tr>\n");

         /* empty title for selection box */
         if (isparam("select") && atoi(getparam("select")) == 1)
            rsprintf("<th class=\"listtitle\">&nbsp;</th>\n");

         for (i = 0; i < n_attr_disp; i++) {
            /* assemble current command line, replace sort statements */
            strlcpy(ref, getparam("cmdline"), sizeof(ref));

            strlcpy(str, disp_attr[i], sizeof(str));
            url_encode(str, sizeof(str));
            if (isparam("sort") && strcmp(getparam("sort"), disp_attr[i]) == 0) {
               subst_param(ref, sizeof(ref), "sort", "");
               subst_param(ref, sizeof(ref), "rsort", str);
            } else {
               if (ref[0] == 0) {
                  if (getcfg(lbs->name, "Reverse sort", str2, sizeof(str2)) && atoi(str2) == 1)
                     sprintf(ref, "?rsort=%s", str);
                  else
                     sprintf(ref, "?sort=%s", str);
               } else {
                  subst_param(ref, sizeof(ref), "rsort", "");
                  subst_param(ref, sizeof(ref), "sort", str);
               }
            }

            img[0] = 0;
            if (isparam("sort") && strcmp(getparam("sort"), disp_attr[i]) == 0)
               sprintf(img, "<img align=top src=\"up.png\" alt=\"%s\" title=\"%s\">", loc("up"), loc("up"));
            else if (isparam("rsort") && strcmp(getparam("rsort"), disp_attr[i]) == 0)
               sprintf(img, "<img align=top src=\"down.png\" alt=\"%s\" title=\"%s\">", loc("down"),
                       loc("down"));

            sprintf(str, "Tooltip %s", disp_attr[i]);
            if (getcfg(lbs->name, str, comment, sizeof(comment)))
               sprintf(str, "title=\"%s\"", comment);
            else
               str[0] = 0;

            if (strieq(disp_attr[i], "Edit") || strieq(disp_attr[i], "Delete"))
               rsprintf("<th %s class=\"listtitle\">%s</th>\n", str, disp_attr[i]);
            else {
               rsprintf("<th %s class=\"listtitle\"><a href=\"", str);
               rsputs3(ref);
               rsprintf("\">%s</a>%s</th>\n", disp_attr[i], img);
            }
         }

         if (!strieq(mode, "Full") && n_line > 0 && show_text)
            rsprintf("<th class=\"listtitle2\">%s</th>\n", loc("Text"));

         if (show_att_column)
            rsprintf("<th class=\"listtitle3\"><img src=\"attachment.png\" alt=\"%s\" title=\"%s\"</th>",
                     loc("Attachments"), loc("Attachments"));

         rsprintf("</tr>\n\n");
      }
   }                            /* if (!csv && !xml) */

   /*---- display message list ----*/

   for (index = i_start; index <= i_stop; index++) {
      size = TEXT_SIZE;
      message_id = msg_list[index].lbs->el_index[msg_list[index].index].message_id;

      status = el_retrieve(msg_list[index].lbs, message_id, date, attr_list, attrib, lbs->n_attr, text,
                           &size, in_reply_to, reply_to, attachment, encoding, locked_by, draft);
      if (status != EL_SUCCESS)
         break;

      /* skip drafts */
      if (getcfg(lbs->name, "List drafts", str, sizeof(str)) && atoi(str) == 0)
         if (draft[0])
            continue;

      if (csv) {

         rsprintf("%d", message_id);
         if (strieq(mode, "CSV1"))
            rsprintf(",");
         else
            rsprintf(";");

         strlcpy(str, date, sizeof(str));
         while (strchr(str, ','))
            *strchr(str, ',') = ' ';
         rsprintf(str);
         if (strieq(mode, "CSV1"))
            rsprintf(",");
         else
            rsprintf(";");

         for (i = 0; i < lbs->n_attr; i++) {
            strlcpy(str, attrib[i], sizeof(str));
            if (str[0]) {

               if (attr_flags[i] & AF_DATE) {

                  sprintf(str, "Date format %s", attr_list[i]);
                  if (!getcfg(lbs->name, str, format, sizeof(format)))
                     if (!getcfg(lbs->name, "Date format", format, sizeof(format)))
                        strcpy(format, DEFAULT_DATE_FORMAT);

                  ltime = atoi(attrib[i]);
                  ptms = localtime(&ltime);
                  assert(ptms);
                  if (ltime == 0)
                     strcpy(str, "-");
                  else
                     my_strftime(str, sizeof(str), format, ptms);

               } else if (attr_flags[i] & AF_DATETIME) {

                  sprintf(str, "Time format %s", attr_list[i]);
                  if (!getcfg(lbs->name, str, format, sizeof(format)))
                     if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
                        strcpy(format, DEFAULT_TIME_FORMAT);

                  ltime = atoi(attrib[i]);
                  ptms = localtime(&ltime);
                  assert(ptms);
                  if (ltime == 0)
                     strcpy(str, "-");
                  else
                     my_strftime(str, sizeof(str), format, ptms);

               }

               rsputs("\"");
               pt1 = str;
               while ((pt2 = strchr(pt1, '"')) != NULL) {
                  *pt2 = 0;
                  rsputs(pt1);
                  rsputs("\"\"");
                  pt1 = pt2 + 1;
               }
               rsputs(pt1);
               rsputs("\"");
            }
            if (i < lbs->n_attr - 1) {
               if (strieq(mode, "CSV1"))
                  rsprintf(",");
               else
                  rsprintf(";");
            } else {
               if (strlen(text) > 0 && strieq(mode, "CSV3")) {
                  rsprintf(";");
                  strlcpy(str, text, sizeof(str));
                  rsputs("\"");
                  pt1 = str;
                  while ((pt2 = strchr(pt1, '"')) != NULL) {
                     *pt2 = 0;
                     rsputs(pt1);
                     rsputs("\"\"");
                     pt1 = pt2 + 1;
                  }
                  rsputs(pt1);
                  rsputs("\"");
               }
               rsprintf("\r\n");
            }
         }

      } else if (xml) {

         rsputs("\t<ENTRY>\n");
         rsprintf("\t\t<MID>%d</MID>\n", message_id);
         rsprintf("\t\t<DATE>%s</DATE>\n", date);
         if (in_reply_to[0])
            rsprintf("\t\t<IN_REPLY_TO>%s</IN_REPLY_TO>\n", in_reply_to);
         if (reply_to[0])
            rsprintf("\t\t<REPLY_TO>%s</REPLY_TO>\n", reply_to);
         if (attachment[0][0]) {
            rsprintf("\t\t<ATTACHMENT>");
            rsprintf(attachment[0]);
            for (i = 1; i < MAX_ATTACHMENTS; i++)
               if (attachment[i][0])
                  rsprintf(",%s", attachment[i]);
            rsprintf("</ATTACHMENT>\n", attachment);
         }
         rsprintf("\t\t<ENCODING>%s</ENCODING>\n", encoding);

         for (i = 0; i < lbs->n_attr; i++) {
            strcpy(iattr, attr_list[i]);
            for (j = 0; j < (int) strlen(iattr); j++)
               /* replace special characters with "_", exclude any UTF-8 */
               if (!isalnum(iattr[j]) && ((unsigned char) iattr[j] < 128))
                  iattr[j] = '_';
            rsprintf("\t\t<%s>", iattr);

            strlcpy(str, attrib[i], sizeof(str));
            if (attr_flags[i] & AF_DATE) {

               sprintf(str, "Date format %s", attr_list[i]);
               if (!getcfg(lbs->name, str, format, sizeof(format)))
                  if (!getcfg(lbs->name, "Date format", format, sizeof(format)))
                     strcpy(format, DEFAULT_DATE_FORMAT);

               ltime = atoi(attrib[i]);
               ptms = localtime(&ltime);
               assert(ptms);
               if (ltime == 0)
                  strcpy(str, "-");
               else
                  my_strftime(str, sizeof(str), format, ptms);
               sprintf(str + strlen(str), " [%ld]", (long)ltime);

            } else if (attr_flags[i] & AF_DATETIME) {

               sprintf(str, "Time format %s", attr_list[i]);
               if (!getcfg(lbs->name, str, format, sizeof(format)))
                  if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
                     strcpy(format, DEFAULT_TIME_FORMAT);

               ltime = atoi(attrib[i]);
               ptms = localtime(&ltime);
               assert(ptms);
               if (ltime == 0)
                  strcpy(str, "-");
               else
                  my_strftime(str, sizeof(str), format, ptms);
               sprintf(str + strlen(str), " [%ld]", (long)ltime);
            }

            xmlencode(str);
            rsprintf("</%s>\n", iattr);
         }
         rsputs("\t\t<TEXT>");
         xmlencode(text);
         rsputs("</TEXT>\n");

         rsputs("\t</ENTRY>\n");

      } else if (raw) {

         rsprintf("$@MID@$: %d\r\n", message_id);
         rsprintf("Date: %s\r\n", date);

         if (reply_to[0])
            rsprintf("Reply to: %s\r\n", reply_to);

         if (in_reply_to[0])
            rsprintf("In reply to: %s\r\n", in_reply_to);

         for (i = 0; i < lbs->n_attr; i++)
            rsprintf("%s: %s\r\n", attr_list[i], attrib[i]);

         rsprintf("Attachment: ");

         if (attachment[0][0]) {
            rsprintf("%s", attachment[0]);
            for (i = 1; i < MAX_ATTACHMENTS; i++)
               if (attachment[i][0])
                  rsprintf(",%s", attachment[i]);
         }
         rsprintf("\r\n");

         rsprintf("Encoding: %s\r\n", encoding);
         if (locked_by[0])
            rsprintf("Locked by: %s\r\n", locked_by);

         rsprintf("========================================\r\n");
         rsputs(text);
         rsputs("\r\n");

      } else {

         /*---- add highlighting for searched subtext ----*/

         if (isparam("subtext")) {
            highlight_searchtext(re_buf, text, text1, strieq(encoding, "plain") || strieq(encoding, "ELCode")
                                                      || !strieq(mode, "Full"));
            strlcpy(text, text1, TEXT_SIZE);
         }

         /*---- display line ----*/

         expand = 1;
         if (threaded) {
            if (getcfg(lbs->name, "Expand default", str, sizeof(str)))
               expand = atoi(str);

            if (isparam("expand"))
               expand = atoi(getparam("expand"));
         }

         level = 0;
         if (!filtering && !date_filtering) {
            if (expand == 0 && (!getcfg(lbs->name, "Collapse to last", str, sizeof(str)) || atoi(str) == 1)) {
               /* search last entry in this thread */
               if (reply_to[0]) {
                  search_last_reply(msg_list[index].lbs, &message_id);
                  size = TEXT_SIZE;
                  status =
                          el_retrieve(msg_list[index].lbs, message_id, date, attr_list, attrib, lbs->n_attr,
                                      text,
                                      &size, in_reply_to, reply_to, attachment, encoding, locked_by, draft);
                  if (status == SUCCESS)
                     level = 1;
               }
            }
         } else if (in_reply_to[0])
            level = 1;

         display_line(msg_list[index].lbs, message_id, index, mode, expand, level, printable, n_line,
                      show_attachments, show_att_column, date, in_reply_to, reply_to, n_attr_disp, disp_attr,
                      disp_attr_link, attrib, lbs->n_attr, text, show_text, attachment, encoding,
                      isparam("select") ? atoi(getparam("select")) : 0, &n_display, locked_by, 0, re_buf,
                      page_mid, FALSE, draft);

         if (threaded && !filtering && !date_filtering) {
            if (reply_to[0] && expand > 0) {
               p = reply_to;
               do {
                  display_reply(msg_list[index].lbs, atoi(p), printable, expand, n_line, n_attr_disp,
                                disp_attr, show_text, 1, 0, re_buf, page_mid, FALSE);

                  while (*p && isdigit(*p))
                     p++;
                  while (*p && (*p == ',' || *p == ' '))
                     p++;
               } while (*p);
            }
         }
      }                         /* if (!csv && !xml) */
   }                            /* for() */

   if (!csv && !xml && !raw) {
      rsprintf("</table>\n");

      if (n_display)
         rsprintf("<input type=hidden name=nsel value=%d>\n", n_display);

      rsprintf("</td></tr>\n");

      if (n_msg == 0)
         rsprintf("<tr><td class=\"errormsg\">%s</td></tr>", loc("No entries found"));

      /*---- page navigation ----*/

      if (!printable)
         show_page_navigation(lbs, n_msg, page_n, n_page);

      rsprintf("</table>\n");
      show_bottom_text(lbs);
      rsprintf("</form></body></html>\r\n");
   }

   if (xml) {
      rsputs("</ELOG_LIST>\n");
   }

   regfree(re_buf);
   for (i = 0; i < lbs->n_attr; i++)
      regfree(re_buf + 1 + i);

   xfree(slist);
   xfree(svalue);
   xfree(gattr);
   xfree(list);
   xfree(msg_list);
   xfree(text);
   xfree(text1);
}