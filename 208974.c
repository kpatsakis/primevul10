void submit_elog(LOGBOOK *lbs) {
   char str[NAME_LENGTH], str2[NAME_LENGTH], file_name[256], error[1000], date[80], *mail_list, *rcpt_list,
           list[10000], *p, locked_by[256], encoding[80], attrib[MAX_N_ATTR][NAME_LENGTH],
           subst_str[MAX_PATH_LENGTH], in_reply_to[80], reply_to[MAX_REPLY_TO * 10], user[256],
           user_email[256], mail_param[1000], *mail_to, *rcpt_to, full_name[256],
           att_file[MAX_ATTACHMENTS][256], slist[MAX_N_ATTR + 10][NAME_LENGTH],
           svalue[MAX_N_ATTR + 10][NAME_LENGTH], ua[NAME_LENGTH], draft[256];
   int i, j, k, n, missing, first, index, mindex, suppress, message_id, resubmit_orig, mail_to_size,
           rcpt_to_size, ltime, year, month, day, hour, min, sec, n_attr, email_notify[1000], allowed_encoding,
           status, bdraft, old_mail;
   BOOL bedit, bmultiedit;
   struct tm tms;

   bmultiedit = isparam("nsel");
   bedit = isparam("edit_id") && atoi(getparam("edit_id"));
   bdraft = isparam("draft");

   /* check for condition */
   if (isparam("condition")) {
      set_condition(getparam("condition"));

      /* rescan attributes */
      n_attr = scan_attributes(lbs->name);
   } else
      n_attr = lbs->n_attr;

   /* check for editing interval */
   if (isparam("edit_id"))
      if (!check_edit_time(lbs, atoi(getparam("edit_id"))))
         return;

   /* check for required attributs */
   missing = 0;
   for (i = 0; i < lbs->n_attr; i++) {
      strcpy(ua, attr_list[i]);
      stou(ua);

      if (attr_flags[i] & AF_REQUIRED) {
         if (attr_flags[i] & AF_DATE) {
            sprintf(str, "d%d", i);
            if (isparam(str) == 0)
               missing = 1;
            sprintf(str, "m%d", i);
            if (isparam(str) == 0)
               missing = 1;
            sprintf(str, "y%d", i);
            if (isparam(str) == 0)
               missing = 1;
            if (missing)
               break;
         } else if (attr_flags[i] & AF_DATETIME) {
            sprintf(str, "d%d", i);
            if (isparam(str) == 0)
               missing = 1;
            sprintf(str, "m%d", i);
            if (isparam(str) == 0)
               missing = 1;
            sprintf(str, "y%d", i);
            if (isparam(str) == 0)
               missing = 1;
            sprintf(str, "h%d", i);
            if (isparam(str) == 0)
               missing = 1;
            sprintf(str, "n%d", i);
            if (isparam(str) == 0)
               missing = 1;
            if (missing)
               break;
         } else if ((attr_flags[i] & (AF_MULTI | AF_MUSERLIST | AF_MUSEREMAIL))) {
            for (j = 0; j < MAX_N_LIST; j++) {
               sprintf(str, "%s_%d", ua, j);
               if (isparam(str))
                  break;

               /* check for attributes without the _<i> from elog */
               if (isparam(ua))
                  break;
            }

            if (j == MAX_N_LIST) {
               missing = 1;
               break;
            }
         } else if (isparam(ua) == 0 || *getparam(ua) == 0) {
            missing = 1;
            break;
         }
      }
   }

   if (missing && !bdraft) {
      sprintf(error, "<i>");
      sprintf(error + strlen(error), loc("Error: Attribute <b>%s</b> not supplied"), attr_list[i]);
      sprintf(error + strlen(error), ".</i><p>\n");
      sprintf(error + strlen(error), loc("Please go back and enter the <b>%s</b> field"), attr_list[i]);
      strcat(error, ".\n");

      show_error(error);
      return;
   }

   /* check for numeric attributes */
   if (!bdraft) {
      for (index = 0; index < lbs->n_attr; index++)
         if (attr_flags[index] & AF_NUMERIC) {
            strcpy(ua, attr_list[index]);
            stou(ua);
            strlcpy(str, isparam(ua) ? getparam(ua) : "", sizeof(str));

            for (j = 0; i < (int) strlen(str); i++)
               if (!isdigit(str[i]))
                  break;

            sprintf(str2, "- %s -", loc("keep original values"));
            if (i < (int) strlen(str) && strcmp(str, "<keep>") != 0 && strcmp(str, str2) != 0) {
               sprintf(error, loc("Error: Attribute <b>%s</b> must be numeric"), attr_list[index]);
               show_error(error);
               return;
            }
         }
   }

   for (i = 0; i < n_attr; i++) {
      strcpy(ua, attr_list[i]);
      stou(ua);
      if (attr_flags[i] & (AF_MULTI | AF_MUSERLIST | AF_MUSEREMAIL))
         strcat(ua, "_0");

      if (isparam(ua) && *getparam(ua) && attr_options[i][0][0]) {

         if (strieq(attr_options[i][0], "boolean") && !bdraft) {
            if (atoi(getparam(ua)) != 0 && atoi(getparam(ua)) != 1 && strcmp(getparam(ua), "<keep>") != 0) {
               strencode2(str, getparam(ua), sizeof(str));
               sprintf(error, loc("Error: Value <b>%s</b> not allowed for boolean attributes"), str);
               show_error(error);
               return;
            }

         } else {

            /* check if option exists */
            for (j = 0; attr_options[i][j][0]; j++)
               if (strieq(attr_options[i][j], getparam(ua)))
                  break;

            /* check if option without {n} exists */
            if (!attr_options[i][j][0]) {
               for (j = 0; attr_options[i][j][0]; j++) {
                  strlcpy(str, attr_options[i][j], sizeof(str));
                  if (strchr(str, '{'))
                     *strchr(str, '{') = 0;
                  if (strieq(str, getparam(ua)))
                     break;
               }
            }

            if (!attr_options[i][j][0] && isparam(ua) && strcmp(getparam(ua), "<keep>") != 0) {
               if (attr_flags[i] & AF_EXTENDABLE) {

                  /* check if maximal number of options exceeded */
                  if (attr_options[i][MAX_N_LIST - 1][0]) {
                     strcpy(error, loc("Maximum number of attribute options exceeded"));
                     strcat(error, "<br>");
                     strcat(error, loc("Please increase MAX_N_LIST in elogd.c and recompile"));
                     show_error(error);
                     return;
                  }

                  if (!add_attribute_option(lbs, attr_list[i], getparam(ua), getparam("condition")))
                     return;
               } else {
                  char encoded[100];
                  strencode2(encoded, getparam(ua), sizeof(encoded));
                  sprintf(error, loc("Error: Attribute option <b>%s</b> not existing"), encoded);
                  show_error(error);
                  return;
               }
            }
         }
      }
   }

   /* check if allowed encoding */
   if (getcfg(lbs->name, "Allowed encoding", str, sizeof(str)))
      allowed_encoding = atoi(str);
   else
      allowed_encoding = 7;

   strlcpy(encoding, isparam("encoding") ? getparam("encoding") : "plain", sizeof(encoding));

   /* check for valid encoding */
   if (!strieq(encoding, "plain") && !strieq(encoding, "ELCode") && !strieq(encoding, "HTML"))
      strcpy(encoding, "plain");

   if (strieq(encoding, "plain") && (allowed_encoding & 1) == 0) {
      show_error("Plain encoding not allowed");
      return;
   }
   if (strieq(encoding, "ELCode") && (allowed_encoding & 2) == 0) {
      show_error("ELCode encoding not allowed");
      return;
   }
   if (strieq(encoding, "HTML") && (allowed_encoding & 4) == 0) {
      show_error("HTML encoding not allowed");
      return;
   }

   /* get attachments */
   for (i = 0; i < MAX_ATTACHMENTS; i++) {
      sprintf(str, "attachment%d", i);
      strcpy(att_file[i], isparam(str) ? getparam(str) : "");
   }

   /* retrieve attributes */
   for (i = 0; i < n_attr; i++) {

      strcpy(ua, attr_list[i]);
      stou(ua);

      if (strieq(attr_options[i][0], "boolean") && !isparam(ua)) {

         strcpy(attrib[i], "0");

      } else if (attr_flags[i] & (AF_MULTI | AF_MUSERLIST | AF_MUSEREMAIL)) {

         if (isparam(ua)) {
            strlcpy(attrib[i], getparam(ua), NAME_LENGTH);
         } else {
            attrib[i][0] = 0;
            first = 1;
            for (j = 0; j < MAX_N_LIST; j++) {
               sprintf(str, "%s_%d", ua, j);
               if (isparam(str)) {
                  if (first)
                     first = 0;
                  else
                     strlcat(attrib[i], " | ", NAME_LENGTH);
                  if (strlen(attrib[i]) + strlen(getparam(str)) < NAME_LENGTH - 2)
                     strlcat(attrib[i], getparam(str), NAME_LENGTH);
                  else
                     break;
               }
            }
         }
      } else if (attr_flags[i] & AF_DATE) {

         if (isparam(ua))       // from edit/reply of fixed attributes
            strlcpy(attrib[i], getparam(ua), NAME_LENGTH);
         else {

            sprintf(str, "m%d", i);
            if (isparam(str) && strieq(getparam(str), "<keep>"))
               strcpy(attrib[i], "<keep>");
            else {
               sprintf(str, "y%d", i);
               year = isparam(str) ? atoi(getparam(str)) : 0;
               if (year < 100)
                  year += 2000;

               sprintf(str, "m%d", i);
               month = isparam(str) ? atoi(getparam(str)) : 0;

               sprintf(str, "d%d", i);
               day = isparam(str) ? atoi(getparam(str)) : 0;

               if (month == 0 || day == 0)
                  strcpy(attrib[i], "");
               else {
                  memset(&tms, 0, sizeof(struct tm));
                  tms.tm_year = year - 1900;
                  tms.tm_mon = month - 1;
                  tms.tm_mday = day;
                  tms.tm_hour = 12;

                  ltime = (int) mktime(&tms);

                  if (ltime == -1) {
                     show_error(loc("Date must be between 1970 and 2037"));
                     return;
                  }
                  sprintf(attrib[i], "%d", ltime);
               }
            }
         }

      } else if (attr_flags[i] & AF_DATETIME) {

         if (isparam(ua))       // from edit/reply of fixed attributes
            strlcpy(attrib[i], getparam(ua), NAME_LENGTH);
         else {

            sprintf(str, "m%d", i);
            if (isparam(str) && strieq(getparam(str), "<keep>"))
               strcpy(attrib[i], "<keep>");
            else {
               sprintf(str, "y%d", i);
               year = isparam(str) ? atoi(getparam(str)) : 0;
               if (year < 100)
                  year += 2000;

               sprintf(str, "m%d", i);
               month = isparam(str) ? atoi(getparam(str)) : 0;

               sprintf(str, "d%d", i);
               day = isparam(str) ? atoi(getparam(str)) : 0;

               sprintf(str, "h%d", i);
               hour = isparam(str) ? atoi(getparam(str)) : 0;

               sprintf(str, "n%d", i);
               min = isparam(str) ? atoi(getparam(str)) : 0;

               sprintf(str, "c%d", i);
               sec = isparam(str) ? atoi(getparam(str)) : 0;

               if (month == 0 || day == 0)
                  strcpy(attrib[i], "");
               else {
                  memset(&tms, 0, sizeof(struct tm));
                  tms.tm_year = year - 1900;
                  tms.tm_mon = month - 1;
                  tms.tm_mday = day;
                  tms.tm_hour = hour;
                  tms.tm_min = min;
                  tms.tm_sec = sec;
                  tms.tm_isdst = -1;

                  ltime = (int) mktime(&tms);

                  if (ltime == -1) {
                     show_error(loc("Date must be between 1970 and 2037"));
                     return;
                  }
                  sprintf(attrib[i], "%d", ltime);
               }
            }
         }

      } else {
         strlcpy(attrib[i], isparam(ua) ? getparam(ua) : "", NAME_LENGTH);

         /* remove any CR/LF */
         if (strchr(attrib[i], '\r'))
            *strchr(attrib[i], '\r') = 0;
         if (strchr(attrib[i], '\n'))
            *strchr(attrib[i], '\n') = 0;

         /* strip trailing "{...}" */
         if (is_cond_attr(i) && strchr(attrib[i], '{') && strchr(attrib[i], '}'))
            *strchr(attrib[i], '{') = 0;
      }

   }

   /* compile substitution list */
   n = build_subst_list(lbs, slist, svalue, attrib, TRUE);
   if (bedit)
      add_subst_list(slist, svalue, "message id", getparam("edit_id"), &n);

   /* substitute attributes */
   if (!bedit && !isparam("reply_to")) {
      for (index = 0; index < n_attr; index++) {
         sprintf(str, "Subst %s", attr_list[index]);
         if (getcfg(lbs->name, str, subst_str, sizeof(subst_str))) {

            /* do not format date for date attributes */
            i = build_subst_list(lbs, slist, svalue, attrib, (attr_flags[index] & (AF_DATE | AF_DATETIME))
                                                             == 0);

            strsubst_list(subst_str, sizeof(subst_str), slist, svalue, i);

            /* check for index substitution if not in edit mode */
            if (!bedit && strchr(subst_str, '#')) {
               /* get index */
               get_auto_index(lbs, index, subst_str, str, sizeof(str));
               strcpy(subst_str, str);
            }
            strcpy(attrib[index], subst_str);
         }
      }
   }

   /* subst attributes for edits */
   if (bedit) {
      for (index = 0; index < n_attr; index++) {
         sprintf(str, "Subst on edit %s", attr_list[index]);
         if (getcfg(lbs->name, str, str2, sizeof(str2))) {

            /* do not format date for date attributes */
            i = build_subst_list(lbs, slist, svalue, attrib, (attr_flags[index] & (AF_DATE | AF_DATETIME))
                                                             == 0);

            add_subst_list(slist, svalue, "message id", getparam("edit_id"), &i);

            strsubst_list(str2, sizeof(str2), slist, svalue, i);
            if (strlen(str2) > NAME_LENGTH - 100) {
               if (strstr(str2 + 100, "<br>")) {
                  strlcpy(str, strstr(str2 + 100, "<br>"), sizeof(str));
               } else
                  strlcpy(str, str2 + 100, sizeof(str));

               strcpy(str2, "...");
               strlcat(str2, str, sizeof(str2));
            }
            if (strncmp(str2, "<br>", 4) == 0)
               strcpy(attrib[index], str2 + 4);
            else
               strcpy(attrib[index], str2);
         }
      }
   }

   /* subst attributes for replies */
   if (isparam("reply_to")) {
      for (index = 0; index < n_attr; index++) {
         sprintf(str, "Subst on reply %s", attr_list[index]);
         if (getcfg(lbs->name, str, str2, sizeof(str2))) {
            /* do not format date for date attributes */
            i = build_subst_list(lbs, slist, svalue, attrib, (attr_flags[index] & (AF_DATE | AF_DATETIME))
                                                             == 0);
            if (isparam("reply_to"))
               add_subst_list(slist, svalue, "Reply to", getparam("reply_to"), &i);
            strsubst_list(str2, sizeof(str2), slist, svalue, i);
            strcpy(attrib[index], str2);
         }
      }
   }

   /* check for attributes to keep */
   if (bmultiedit) {
      sprintf(str, "- %s -", loc("keep original values"));
      for (i = 0; i < n_attr; i++) {
         if (strieq(str, attrib[i]))
            strlcpy(attrib[i], "<keep>", NAME_LENGTH);
      }
   }

   message_id = 0;
   reply_to[0] = 0;
   in_reply_to[0] = 0;
   date[0] = 0;
   resubmit_orig = 0;
   locked_by[0] = 0;
   draft[0] = 0;
   if (isparam("draft"))
      strlcpy(draft, getparam("draft"), sizeof(draft));

   if (bedit && isparam("resubmit") && atoi(getparam("resubmit")) == 1) {
      resubmit_orig = atoi(getparam("edit_id"));

      /* get old links */
      el_retrieve(lbs, resubmit_orig, NULL, NULL, NULL, 0, NULL, 0, in_reply_to, reply_to, NULL, NULL, NULL,
                  NULL);

      /* if not message head, move all preceeding messages */
      /* outcommented, users want only resubmitted message occur at end (see what's new)
         if (in_reply_to[0])
         {
         do
         {
         resubmit_orig = atoi(in_reply_to);
         el_retrieve(lbs, resubmit_orig, NULL, NULL, NULL, 0,
         NULL, 0, in_reply_to, reply_to, NULL, NULL);
         } while (in_reply_to[0]);
         }
       */

      message_id = atoi(getparam("edit_id"));
      strcpy(in_reply_to, "<keep>");
      strcpy(reply_to, "<keep>");
      date[0] = 0;
   } else {
      if (bedit) {
         message_id = atoi(getparam("edit_id"));
         strcpy(in_reply_to, "<keep>");
         strcpy(reply_to, "<keep>");
         strcpy(date, "<keep>");
         if (bdraft)
            strcpy(locked_by, "<keep>");
      } else
         strcpy(in_reply_to, isparam("reply_to") ? getparam("reply_to") : "");
   }

   if (_logging_level > 1) {
      if (bmultiedit)
         sprintf(str, "EDIT multiple entries");
      else if (bdraft)
         sprintf(str, "DRAFT entry #%d", message_id);
      else if (bedit && !resubmit_orig)
         sprintf(str, "EDIT entry #%d", message_id);
      else
         sprintf(str, "NEW entry #%d", message_id);

      write_logfile(lbs, str);
   }

   if (bmultiedit) {
      for (i = n = 0; i < atoi(getparam("nsel")); i++) {
         sprintf(str, "s%d", i);
         if (isparam(str)) {

            message_id = atoi(getparam(str));

            status = submit_elog_reply(lbs, message_id, attrib, getparam("text"));
            if (status != EL_SUCCESS) {
               sprintf(str, loc("New entry cannot be written to directory \"%s\""), lbs->data_dir);
               strcat(str, "\n<p>");
               strcat(str,
                      loc("Please check that it exists and elogd has write access and disk is not full"));
               show_error(str);
               return;
            }
         }
      }

      redirect(lbs, isparam("redir") ? getparam("redir") : "");
      return;                   /* no email notifications etc */
   } else {
      message_id = el_submit(lbs, message_id, bedit, date, attr_list, attrib, n_attr, getparam("text"),
                             in_reply_to, reply_to, encoding, att_file, TRUE, locked_by, draft);

      if (message_id <= 0) {
         sprintf(str, loc("New entry cannot be written to directory \"%s\""), lbs->data_dir);
         strcat(str, "\n<p>");
         strcat(str, loc("Please check that it exists and elogd has write access and disk is not full"));
         show_error(str);
         return;
      }

      if (bdraft) {
         show_http_header(lbs, FALSE, NULL);
         rsprintf("OK %d\n", message_id);
         return;
      }
   }

   /* evaluate propagation of attributes */
   if (getcfg(lbs->name, "Propagate attributes", str, sizeof(str)))
      propagate_attrib(lbs, find_thread_head(lbs, message_id), attrib);

   /* resubmit thread if requested */
   if (resubmit_orig)
      message_id = el_move_message_thread(lbs, resubmit_orig);

   /* retrieve submission date */
   if (date[0] == 0)
      el_retrieve(lbs, message_id, date, NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

   /*---- replace relative elog:/x link by elog:n/x link */
   if (stristr(getparam("text"), "elog:/")) {
      p = getparam("text");
      if (stricmp(encoding, "HTML") == 0) {
         sprintf(str, "%d/", message_id);
      } else
         sprintf(str, "elog:%d/", message_id);
      strsubst(p, TEXT_SIZE, "elog:/", str);
      el_submit(lbs, message_id, TRUE, date, attr_list, attrib, n_attr, p, in_reply_to, reply_to, encoding,
                att_file, TRUE, NULL, NULL);
   }

   /*---- replace elog: by HTML link ----*/
   if (strieq(encoding, "HTML") && stristr(getparam("text"), "elog:")) {
      p = stristr(getparam("text"), "elog:");
      while (p) {
         for (i = 0; i < 5 || (p[i] == '/' || isalnum(p[i])); i++)
            str[i] = p[i];
         str[i] = 0;
         convert_elog_link(lbs, str + 5, str + 5, str2, 0, message_id);
         strsubst(p, TEXT_SIZE, str, str2);
         p += strlen(str2);
         p = stristr(p, "elog:");
      }
      el_submit(lbs, message_id, TRUE, date, attr_list, attrib, n_attr, getparam("text"), in_reply_to,
                reply_to, encoding, att_file, TRUE, NULL, NULL);
   }

   /*---- email notifications ----*/

   suppress = isparam("suppress") ? atoi(getparam("suppress")) : 0;

   /* check for mail submissions */
   mail_param[0] = 0;
   mail_to = (char *) xmalloc(256);
   mail_to[0] = 0;
   mail_to_size = 256;
   rcpt_to = (char *) xmalloc(256);
   rcpt_to[0] = 0;
   rcpt_to_size = 256;
   mail_list = (char *) xmalloc(MAX_N_EMAIL * NAME_LENGTH);
   rcpt_list = (char *) xmalloc(MAX_N_EMAIL * NAME_LENGTH);

   if (suppress == 1 || suppress == 3) {
      if (suppress == 1)
         strcpy(mail_param, "?suppress=1");
   } else {
      /* go throuch "Email xxx" in configuration file */
      for (index = mindex = 0; index <= n_attr; index++) {

         strcpy(ua, attr_list[index]);
         stou(ua);

         if (index < n_attr) {
            strcpy(str, "Email ");
            if (strchr(attr_list[index], ' '))
               sprintf(str + strlen(str), "\"%s\"", attr_list[index]);
            else
               strlcat(str, attr_list[index], sizeof(str));
            strcat(str, " ");

            if (attr_flags[index] & (AF_MULTI | AF_MUSERLIST | AF_MUSEREMAIL)) {
               sprintf(str2, "%s_%d", ua, mindex);

               mindex++;
               if (mindex == MAX_N_LIST)
                  mindex = 0;
               else
                  index--;      /* repeat this loop */
            } else
               strlcpy(str2, ua, sizeof(str2));

            if (isparam(str2)) {
               if (strchr(getparam(str2), ' ')) {
                  strlcat(str, "\"", sizeof(str));
                  strlcat(str, getparam(str2), sizeof(str));
                  strlcat(str, "\"", sizeof(str));
               } else
                  strlcat(str, getparam(str2), sizeof(str));
            }
         } else
            sprintf(str, "Email ALL");

         if (getcfg(lbs->name, str, list, sizeof(list))) {
            i = build_subst_list(lbs, slist, svalue, attrib, TRUE);
            strsubst_list(list, sizeof(list), slist, svalue, i);

            n = strbreak(list, (char (*)[1500]) mail_list, 200, ",", FALSE);

            if (get_verbose() >= VERBOSE_INFO)
               eprintf("\n%s to %s\n\n", str, list);

            for (i = 0; i < n; i++) {
               /* remove possible 'mailto:' */
               if ((p = strstr(&mail_list[i * NAME_LENGTH], "mailto:")) != NULL)
                  memmove(p, p + 7, strlen(p + 7) + 1);

               if ((int) strlen(mail_to) + (int) strlen(&mail_list[i * NAME_LENGTH]) + 10 >= mail_to_size) {
                  mail_to_size += 256;
                  mail_to = xrealloc(mail_to, mail_to_size);
               }
               strcat(mail_to, &mail_list[i * NAME_LENGTH]);
               strcat(mail_to, ",");

               if ((int) strlen(rcpt_to) + (int) strlen(&mail_list[i * NAME_LENGTH]) + 10 >= rcpt_to_size) {
                  rcpt_to_size += 256;
                  rcpt_to = xrealloc(rcpt_to, rcpt_to_size);
               }
               strcat(rcpt_to, &mail_list[i * NAME_LENGTH]);
               strcat(rcpt_to, ",");
            }
         }
      }

      if (!getcfg(lbs->name, "Suppress Email to users", str, sizeof(str)) || atoi(str) == 0) {
         /* go through password file */
         for (index = 0;; index++) {
            if (!enum_user_line(lbs, index, user, sizeof(user)))
               break;

            get_user_line(lbs, user, NULL, full_name, user_email, email_notify, NULL, NULL);

            for (i = 0; lb_list[i].name[0] && i < 1000; i++)
               if (strieq(lb_list[i].name, lbs->name))
                  break;

            if (email_notify[i]) {
               /* check if user has access to this logbook */
               if (!check_login_user(lbs, user))
                  continue;

               sprintf(str, "\"%s\" <%s>,", full_name, user_email);
               if ((int) strlen(mail_to) + (int) strlen(str) + 1 >= mail_to_size) {
                  mail_to_size += 256;
                  mail_to = xrealloc(mail_to, mail_to_size);
               }
               strcat(mail_to, str);

               sprintf(str, "%s,", user_email);
               if ((int) strlen(rcpt_to) + (int) strlen(str) + 1 >= rcpt_to_size) {
                  rcpt_to_size += 256;
                  rcpt_to = xrealloc(rcpt_to, rcpt_to_size);
               }
               strcat(rcpt_to, str);
            }
         }
      }
   }

   if (strlen(mail_to) > 0) {
      /* convert any '|' to ',', remove duplicate email to's */
      n = strbreak(rcpt_to, (void *) rcpt_list, MAX_N_EMAIL, ",|", TRUE);
      strbreak(mail_to, (void *) mail_list, MAX_N_EMAIL, ",|", TRUE);
      for (i = 0; i < n - 1; i++) {
         for (j = i + 1; j < n; j++) {
            if (rcpt_list[i * NAME_LENGTH] && rcpt_list[j * NAME_LENGTH] &&
                strstr(&rcpt_list[i * NAME_LENGTH], &rcpt_list[j * NAME_LENGTH])) {
               for (k = j; k < n - 1; k++) {
                  memcpy(&rcpt_list[k * NAME_LENGTH], &rcpt_list[(k + 1) * NAME_LENGTH], NAME_LENGTH);
                  memcpy(&mail_list[k * NAME_LENGTH], &mail_list[(k + 1) * NAME_LENGTH], NAME_LENGTH);
               }
               memset(&rcpt_list[k * NAME_LENGTH], 0, NAME_LENGTH);
               memset(&mail_list[k * NAME_LENGTH], 0, NAME_LENGTH);
               j = j - 1;
               n = n - 1;
               break;
            }
         }
      }
      rcpt_to[0] = 0;
      mail_to[0] = 0;
      for (i = 0; i < n; i++) {

         if ((int) strlen(rcpt_to) + (int) strlen(&rcpt_list[i * NAME_LENGTH]) + 5 >= rcpt_to_size) {
            rcpt_to_size += 256;
            rcpt_to = xrealloc(rcpt_to, rcpt_to_size);
         }
         strcat(rcpt_to, &rcpt_list[i * NAME_LENGTH]);

         if ((int) strlen(mail_to) + (int) strlen(&mail_list[i * NAME_LENGTH]) + 5 >= mail_to_size) {
            mail_to_size += 256;
            mail_to = xrealloc(mail_to, mail_to_size);
         }
         strcat(mail_to, &mail_list[i * NAME_LENGTH]);

         if (i < MAX_N_EMAIL - 1 && rcpt_list[(i + 1) * NAME_LENGTH]) {
            strcat(rcpt_to, ",");
            strcat(mail_to, ",\r\n\t");
         }
      }

      /* fix for edited draft messages. new_entry is a hidden field persisting draft cycles */
      old_mail = bedit;
      if (isparam("new_entry"))
         old_mail = 0;
      if (resubmit_orig)
         old_mail = 0;

      if (compose_email(lbs, rcpt_to, mail_to, message_id, attrib, mail_param, old_mail, att_file,
                        isparam("encoding") ? getparam("encoding") : "plain", atoi(in_reply_to)) == 0) {
         xfree(mail_to);
         xfree(rcpt_to);
         xfree(mail_list);
         xfree(rcpt_list);
         return;
      }
   }

   xfree(mail_to);
   xfree(rcpt_to);
   xfree(mail_list);
   xfree(rcpt_list);

   /*---- shell execution ----*/

   if (!(isparam("shell_suppress") && atoi(getparam("shell_suppress")))) {
      if (!bedit) {
         if (getcfg(lbs->name, "Execute new", str, sizeof(str)))
            execute_shell(lbs, message_id, attrib, att_file, str);
      } else {
         if (getcfg(lbs->name, "Execute edit", str, sizeof(str)))
            execute_shell(lbs, message_id, attrib, att_file, str);
      }
   }

   /*---- custom submit page ----*/

   if (getcfg(lbs->name, "Submit page", str, sizeof(str))) {
      /* check if file starts with an absolute directory */
      if (str[0] == DIR_SEPARATOR || str[1] == ':')
         strcpy(file_name, str);
      else {
         strlcpy(file_name, logbook_dir, sizeof(file_name));
         strlcat(file_name, str, sizeof(file_name));
      }
      send_file_direct(file_name);
      return;
   }

   if (getcfg(lbs->name, "List after submit", str, sizeof(str)) && atoi(str) == 1)
      str[0] = 0;
   else
      sprintf(str, "%d%s", message_id, mail_param);
   redirect(lbs, str);
}