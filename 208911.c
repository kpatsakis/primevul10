void show_edit_form(LOGBOOK *lbs, int message_id, BOOL breply, BOOL bedit, BOOL bupload, BOOL breedit,
                    BOOL bduplicate, BOOL bpreview) {
   int i, j, n, index, aindex, size, width, height, fh, length, input_size, input_maxlen,
           format_flags[MAX_N_ATTR], year, month, day, hour, min, sec, n_attr, n_disp_attr, n_lines,
           attr_index[MAX_N_ATTR], enc_selected, show_text, n_moptions, display_inline,
           allowed_encoding, thumb_status, max_n_lines, fixed_text, autosave, new_entry, status;
   char str[2 * NAME_LENGTH], str2[NAME_LENGTH], preset[2 * NAME_LENGTH], *p, *pend, star[80],
           comment[10000], reply_string[256], list[MAX_N_ATTR][NAME_LENGTH], file_name[256], *buffer,
           format[256], date[80], script_onload[256], script_onfocus[256], script_onunload[256],
           attrib[MAX_N_ATTR][NAME_LENGTH], *text, orig_tag[80], reply_tag[MAX_REPLY_TO * 10],
           att[MAX_ATTACHMENTS][256], encoding[80], slist[MAX_N_ATTR + 10][NAME_LENGTH],
           svalue[MAX_N_ATTR + 10][NAME_LENGTH], owner[256], locked_by[256], class_value[80], class_name[80],
           ua[NAME_LENGTH], mid[80], title[256], login_name[256], full_name[256],
           orig_author[256], attr_moptions[MAX_N_LIST][NAME_LENGTH], ref[256], file_enc[256], tooltip[10000],
           enc_attr[NAME_LENGTH], user_email[256], cmd[256], thumb_name[256], thumb_ref[256], **user_list, fid[20],
           upwd[80], subdir[256], draft[256], page_title[300];
   time_t now, ltime;
   char fl[8][NAME_LENGTH];
   struct tm *pts;
   FILE *f;
   BOOL preset_text, subtable;

   for (i = 0; i < MAX_ATTACHMENTS; i++)
      att[i][0] = 0;

   for (i = 0; i < lbs->n_attr; i++)
      attrib[i][0] = 0;

   text = xmalloc(TEXT_SIZE);
   text[0] = 0;
   orig_author[0] = 0;
   orig_tag[0] = 0;
   encoding[0] = 0;
   date[0] = 0;
   locked_by[0] = 0;
   new_entry = 0;

   if (!message_id || breply)
      new_entry = 1;
   if (isparam("new_entry"))
      new_entry = 1;

   /* check for custom form for new entries */
   if (!bedit && getcfg(lbs->name, "Custom new form", str, sizeof(str))) {
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

   /* check for custom form for editing an entry */
   if (bedit && getcfg(lbs->name, "Custom edit form", str, sizeof(str))) {
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

   /* check for file attachment (mhttpd) */
   if (isparam("fa")) {
      strlcpy(att[0], getparam("fa"), 256);

      /* remove any leading directory, to accept only files in the logbook directory ! */
      if (strchr(att[0], DIR_SEPARATOR)) {
         strlcpy(str, att[0], sizeof(str));
         strlcpy(att[0], strrchr(str, DIR_SEPARATOR) + 1, 256);
      }
   }

   if (breedit || bupload) {
      /* get date from parameter */
      if (isparam("entry_date"))
         strlcpy(date, getparam("entry_date"), sizeof(date));

      /* get attributes from parameters */
      attrib_from_param(lbs->n_attr, attrib);

      strlcpy(text, getparam("text"), TEXT_SIZE);

      for (i = 0; i < MAX_ATTACHMENTS; i++) {
         sprintf(str, "attachment%d", i);
         if (isparam(str))
            strlcpy(att[i], getparam(str), 256);
      }

      if (isparam("inlineatt")) {
         for (i = 0; i < MAX_ATTACHMENTS; i++) {
            sprintf(str, "attachment%d", i);
            if (!isparam(str) && isparam("inlineatt")) {
               strlcpy(att[i], getparam("inlineatt"), 256);
               break;
            }
         }
      }

      /* get encoding */
      strlcpy(encoding, isparam("encoding") ? getparam("encoding") : "", sizeof(encoding));
      if (!strieq(encoding, "plain") && !strieq(encoding, "ELCode") && !strieq(encoding, "HTML"))
         strcpy(encoding, "plain");
   } else {
      if (message_id) {
         /* get message for reply/edit */

         size = TEXT_SIZE;
         el_retrieve(lbs, message_id, date, attr_list, attrib, lbs->n_attr, text, &size, orig_tag, reply_tag,
                     att, encoding, locked_by, draft);

         get_author(lbs, attrib, orig_author);

         /* strip attachments on duplicate */
         if (bduplicate)
            memset(att, 0, sizeof(att));
      } else if (isparam("nsel")) {

         /* multi edit: get all entries and check if attributes are the same */
         memset(attrib, 0, sizeof(attrib));
         for (i = n = 0; i < atoi(getparam("nsel")); i++) {
            sprintf(str, "s%d", i);
            if (isparam(str))
               compare_attributes(lbs, atoi(getparam(str)), attrib, &n);
         }
      }
   }

   if (message_id && getcfg(lbs->name, "Use Lock", str, sizeof(str)) && atoi(str) == 1 && locked_by[0]
       && !isparam("steal")) {
      sprintf(str, "%d", message_id);
      sprintf(text, "%s %s", loc("Entry is currently edited by"), locked_by);
      sprintf(cmd, "?cmd=%s&steal=1", loc("Edit"));
      show_query(lbs, loc("Entry is locked"), text, loc("Edit anyhow"), cmd, loc("Cancel"), str);
      return;
   }

   /* Determine encoding */
   if (getcfg(lbs->name, "Allowed encoding", str, sizeof(str)))
      allowed_encoding = atoi(str);
   else
      allowed_encoding = 7;

   enc_selected = 2;            /* Default is HTML */

   if (allowed_encoding == 2)   /* select ELCode if the only one allowed */
      enc_selected = 0;
   else if (allowed_encoding == 1)      /* select plain if the only one allowed */
      enc_selected = 1;
   else if (allowed_encoding == 3)      /* select ELCode if only plain and ELCode allowed */
      enc_selected = 0;

   /* Overwrite from config file */
   if (getcfg(lbs->name, "Default Encoding", str, sizeof(str)))
      enc_selected = atoi(str);

   /* Overwrite from current entry */
   if (encoding[0]) {
      if (encoding[0] == 'E')
         enc_selected = 0;
      else if (encoding[0] == 'p')
         enc_selected = 1;
      else if (encoding[0] == 'H')
         enc_selected = 2;
   }

   show_text = !getcfg(lbs->name, "Show text", str, sizeof(str)) || atoi(str) == 1;

   /* check for preset attributes without any condition */
   set_condition("");
   for (index = 0; index < lbs->n_attr; index++) {

      /* check for preset string */
      sprintf(str, "Preset %s", attr_list[index]);
      if ((i = getcfg(lbs->name, str, preset, sizeof(preset))) > 0) {

         if ((!bedit && !breply && !bduplicate) ||      /* don't subst on edit or reply */
             (breedit && i == 2)) {     /* subst on reedit only if preset is under condition */

            /* check for index substitution */
            if (!bedit && strchr(preset, '#')) {
               /* get index */
               get_auto_index(lbs, index, preset, str, sizeof(str));
               strcpy(preset, str);
            }

            /* do not format date for date attributes */
            i = build_subst_list(lbs, slist, svalue, attrib, (attr_flags[index] & (AF_DATE | AF_DATETIME))
                                                             == 0);
            strsubst_list(preset, sizeof(preset), slist, svalue, i);
            strcpy(attrib[index], preset);
         }
      }

      sprintf(str, "Preset on first reply %s", attr_list[index]);
      if ((i = getcfg(lbs->name, str, preset, sizeof(preset))) > 0 && breply) {
         if (orig_tag[0] == 0) {
            if (!breedit || (breedit && i == 2)) {      /* subst on reedit only if preset is under condition */

               /* check for index substitution */
               if (!bedit && (strchr(preset, '%') || strchr(preset, '#'))) {
                  /* get index */
                  get_auto_index(lbs, index, preset, str, sizeof(str));
                  strcpy(preset, str);
               }

               /* do not format date for date attributes */
               i = build_subst_list(lbs, slist, svalue, attrib, (attr_flags[index] & (AF_DATE | AF_DATETIME))
                                                                == 0);
               strsubst_list(preset, sizeof(preset), slist, svalue, i);
               strcpy(attrib[index], preset);
            }
         }
      }

      sprintf(str, "Preset on reply %s", attr_list[index]);
      if ((i = getcfg(lbs->name, str, preset, sizeof(preset))) > 0 && breply) {

         if (!breedit || (breedit && i == 2)) { /* subst on reedit only if preset is under condition */

            /* check for index substitution */
            if (!bedit && (strchr(preset, '%') || strchr(preset, '#'))) {
               /* get index */
               get_auto_index(lbs, index, preset, str, sizeof(str));
               strcpy(preset, str);
            }

            /* do not format date for date attributes */
            i = build_subst_list(lbs, slist, svalue, attrib, (attr_flags[index] & (AF_DATE | AF_DATETIME))
                                                             == 0);
            strsubst_list(preset, sizeof(preset), slist, svalue, i);
            strcpy(attrib[index], preset);
         }
      }

      sprintf(str, "Preset on edit %s", attr_list[index]);
      if ((i = getcfg(lbs->name, str, preset, sizeof(preset))) > 0 && bedit) {

         if (!breedit || (breedit && i == 2)) { /* subst on reedit only if preset is under condition */

            /* check for index substitution */
            if (!bedit && (strchr(preset, '%') || strchr(preset, '#'))) {
               /* get index */
               get_auto_index(lbs, index, preset, str, sizeof(str));
               strcpy(preset, str);
            }

            /* do not format date for date attributes */
            i = build_subst_list(lbs, slist, svalue, attrib, (attr_flags[index] & (AF_DATE | AF_DATETIME))
                                                             == 0);
            strsubst_list(preset, sizeof(preset), slist, svalue, i);
            strcpy(attrib[index], preset);
         }
      }

      sprintf(str, "Preset on duplicate %s", attr_list[index]);
      if ((i = getcfg(lbs->name, str, preset, sizeof(preset))) > 0 && bduplicate) {

         if (!breedit || (breedit && i == 2)) { /* subst on reedit only if preset is under condition */

            /* check for index substitution */
            if (!bedit && (strchr(preset, '%') || strchr(preset, '#'))) {
               /* get index */
               get_auto_index(lbs, index, preset, str, sizeof(str));
               strcpy(preset, str);
            }

            /* do not format date for date attributes */
            i = build_subst_list(lbs, slist, svalue, attrib, (attr_flags[index] & (AF_DATE | AF_DATETIME))
                                                             == 0);
            strsubst_list(preset, sizeof(preset), slist, svalue, i);
            strcpy(attrib[index], preset);
         }
      }

      /* check for p<attribute> */
      sprintf(str, "p%s", attr_list[index]);
      if (isparam(str))
         strlcpy(attrib[index], getparam(str), NAME_LENGTH);
   }

   /* evaluate conditional attributes */
   evaluate_conditions(lbs, attrib);

   /* rescan attributes if condition set */
   if (_condition[0]) {
      n_attr = scan_attributes(lbs->name);
      if (breedit || bupload)
         attrib_from_param(n_attr, attrib);

      /* now check again for conditional preset */
      for (index = 0; index < lbs->n_attr; index++) {

         /* check for preset string */
         sprintf(str, "Preset %s", attr_list[index]);
         if ((i = getcfg(lbs->name, str, preset, sizeof(preset))) > 0) {

            if ((!bedit && !breply && !bduplicate) ||   /* don't subst on edit or reply */
                (breedit && i == 2)) {  /* subst on reedit only if preset is under condition */

               /* check for index substitution */
               if (!bedit && (strchr(preset, '%') || strchr(preset, '#'))) {
                  /* get index */
                  get_auto_index(lbs, index, preset, str, sizeof(str));
                  strcpy(preset, str);
               }

               /* do not format date for date attributes */
               i = build_subst_list(lbs, slist, svalue, attrib, (attr_flags[index] & (AF_DATE | AF_DATETIME))
                                                                == 0);
               strsubst_list(preset, sizeof(preset), slist, svalue, i);
               strcpy(attrib[index], preset);
            }
         }

         sprintf(str, "Preset on reply %s", attr_list[index]);
         if ((i = getcfg(lbs->name, str, preset, sizeof(preset))) > 0 && breply) {

            if (!breedit || (breedit && i == 2)) {      /* subst on reedit only if preset is under condition */

               /* check for index substitution */
               if (!bedit && (strchr(preset, '%') || strchr(preset, '#'))) {
                  /* get index */
                  get_auto_index(lbs, index, preset, str, sizeof(str));
                  strcpy(preset, str);
               }

               /* do not format date for date attributes */
               i = build_subst_list(lbs, slist, svalue, attrib, (attr_flags[index] & (AF_DATE | AF_DATETIME))
                                                                == 0);
               strsubst_list(preset, sizeof(preset), slist, svalue, i);
               strcpy(attrib[index], preset);
            }
         }

         sprintf(str, "Preset on duplicate %s", attr_list[index]);
         if ((i = getcfg(lbs->name, str, preset, sizeof(preset))) > 0 && bduplicate) {

            if (!breedit || (breedit && i == 2)) {      /* subst on reedit only if preset is under condition */

               /* check for index substitution */
               if (!bedit && (strchr(preset, '%') || strchr(preset, '#'))) {
                  /* get index */
                  get_auto_index(lbs, index, preset, str, sizeof(str));
                  strcpy(preset, str);
               }

               /* do not format date for date attributes */
               i = build_subst_list(lbs, slist, svalue, attrib, (attr_flags[index] & (AF_DATE | AF_DATETIME))
                                                                == 0);
               strsubst_list(preset, sizeof(preset), slist, svalue, i);
               strcpy(attrib[index], preset);
            }
         }
      }

   } else
      // if (_condition[0])
      n_attr = lbs->n_attr;

   /* check for maximum number of replies */
   if (breply) {
      i = 0;
      p = strtok(reply_tag, ",");
      while (p) {
         i++;
         p = strtok(NULL, ",");
      }

      if (i >= MAX_REPLY_TO) {
         sprintf(str, loc("Maximum number of replies (%d) exceeded"), MAX_REPLY_TO);
         show_error(str);
         xfree(text);
         return;
      }
   }

   /* check for non-allowed branching */
   if (breply && getcfg(lbs->name, "Allow branching", str, sizeof(str)) && atoi(str) == 0) {
      if (reply_tag[0]) {
         show_error("Branches are not allowed in this logbook");
         xfree(text);
         return;
      }
   }

   /* check for author */
   if (bedit && getcfg(lbs->name, "Restrict edit", str, sizeof(str)) && atoi(str) == 1) {
      if (!is_author(lbs, attrib, owner)) {
         strencode2(str2, owner, sizeof(str2));
         sprintf(str, loc("Only user <b>%s</b> can edit this entry"), str2);
         show_error(str);
         xfree(text);
         return;
      }
   }


   if (bedit) {
      if (isparam("nsel")) {
         for (i = n = 0; i < atoi(getparam("nsel")); i++) {
            sprintf(str, "s%d", i);
            if (isparam(str)) {
               status = check_edit_time(lbs, atoi(getparam(str)));
               if (!status) {
                  xfree(text);
                  return;
               }
            }
         }
      } else if (message_id) {
         status = check_edit_time(lbs, message_id);
         if (!status) {
            xfree(text);
            return;
         }
      }
   }

   /* check for locking */
   if (message_id && bedit && !breedit && !bupload) {
      if (getcfg(lbs->name, "Use Lock", str, sizeof(str)) && atoi(str) == 1) {
         if (isparam("unm"))
            get_full_name(lbs, getparam("unm"), str);
         else
            strlcpy(str, loc("user"), sizeof(str));

         strcat(str, " ");
         strcat(str, loc("on"));
         strcat(str, " ");
         strcat(str, rem_host);

         el_lock_message(lbs, message_id, str, TRUE);
      }
   }

   /* remove attributes for replies */
   if (breply) {
      getcfg(lbs->name, "Remove on reply", str, sizeof(str));
      n = strbreak(str, list, MAX_N_ATTR, ",", FALSE);
      for (i = 0; i < n; i++)
         for (j = 0; j < n_attr; j++) {
            if (strieq(attr_list[j], list[i]))
               attrib[j][0] = 0;
         }
   }

   /* header */

   if (getcfg(lbs->name, "Edit Page Title", str, sizeof(str))) {
      i = build_subst_list(lbs, (char (*)[NAME_LENGTH]) slist, (char (*)[NAME_LENGTH]) svalue, NULL, TRUE);
      strsubst_list(page_title, sizeof(page_title), (char (*)[NAME_LENGTH]) slist,
                    (char (*)[NAME_LENGTH]) svalue, i);
      strip_html(page_title);
   } else
      sprintf(page_title, "ELOG %s", lbs->name);

   show_html_header(lbs, FALSE, page_title, FALSE, FALSE, NULL, FALSE, 0);

   /* java script for checking required attributes and to check for cancelled edits */
   rsprintf("<script type=\"text/javascript\">\n");
   rsprintf("<!--\n\n");
   rsprintf("var submitted = false;\n");

   if (breedit) {
      if (isparam("entry_modified") && atoi(getparam("entry_modified")) == 1) {
         rsprintf("var entry_modified = true;\n");
         rsprintf("window.status = \"%s\";\n", loc("Entry has been modified"));
      } else
         rsprintf("var entry_modified = false;\n");
   } else
      rsprintf("var entry_modified = false;\n");
   rsprintf("var draft_modified = false;\n");
   rsprintf("var last_key = 0;\n\n");
   rsprintf("var in_asend = false;\n\n");

   rsprintf("function chkform(button)\n");
   rsprintf("{\n");

   rsprintf("  if (last_key == 13) {\n");
   rsprintf("    var ret = confirm('%s');\n", loc("Really submit this entry?"));
   rsprintf("    if (!ret) {\n");
   rsprintf("      last_key = 0;\n");
   rsprintf("      return false;\n");
   rsprintf("    }\n");
   rsprintf("  }\n\n");

   for (i = 0; i < n_attr; i++) {
      if ((attr_flags[i] & AF_REQUIRED) && !(attr_flags[i] & AF_LOCKED)) {

         /* convert blanks etc. to underscores */
         strcpy(ua, attr_list[i]);
         stou(ua);

         rsprintf("  try {\n"); // try-catch block in case attributes are not there (conditional)

         if (attr_flags[i] & AF_MULTI) {
            rsprintf("  if (\n");
            for (j = 0; j < MAX_N_LIST && attr_options[i][j][0]; j++) {
               sprintf(str, "%s_%d", ua, j);
               rsprintf("      !document.form1.%s.checked", str);
               if (attr_options[i][j + 1][0])
                  rsprintf(" &&\n");
            }
            rsprintf(") {\n");
            sprintf(str, loc("Please select at least one '%s'"), attr_list[i]);
            rsprintf("      alert(\"%s\");\n", str);
            rsprintf("      document.form1.%s_0.focus();\n", ua);
            rsprintf("      return false;\n");
            rsprintf("    }\n");

         } else if (attr_flags[i] & (AF_MUSERLIST | AF_MUSEREMAIL)) {
            rsprintf("  if (\n");
            for (j = 0;; j++) {
               if (!enum_user_line(lbs, j, login_name, sizeof(login_name)))
                  break;
               get_user_line(lbs, login_name, NULL, NULL, NULL, NULL, NULL, NULL);

               sprintf(str, "%s_%d", ua, j);
               rsprintf("      !document.form1.%s.checked", str);
               if (enum_user_line(lbs, j + 1, login_name, sizeof(login_name)))
                  rsprintf(" &&\n");
            }
            rsprintf(") {\n");
            sprintf(str, loc("Please select at least one '%s'"), attr_list[i]);
            rsprintf("      alert(\"%s\");\n", str);
            rsprintf("      document.form1.%s_0.focus();\n", ua);
            rsprintf("      return false;\n");
            rsprintf("    }\n");

         } else if (attr_flags[i] & AF_RADIO) {
            rsprintf("    for (var i=0 ; i<document.form1.%s.length ; i++)\n", ua);
            rsprintf("      if (document.form1.%s[i].checked) { break }\n", ua);
            rsprintf("    if (i == document.form1.%s.length) {\n", ua);
            sprintf(str, loc("Please select a '%s'"), attr_list[i]);
            rsprintf("      alert(\"%s\");\n", str);
            rsprintf("      document.form1.%s[0].focus();\n", ua);
            rsprintf("      return false;\n");
            rsprintf("    }\n");

         } else if (attr_flags[i] & (AF_DATE | AF_DATETIME)) {
            rsprintf("    if (document.form1.m%d.value == \"\") {\n", i);
            sprintf(str, loc("Please enter month for attribute '%s'"), attr_list[i]);
            rsprintf("      alert(\"%s\");\n", str);
            rsprintf("      document.form1.m%d.focus();\n", i);
            rsprintf("      return false;\n");
            rsprintf("    }\n");
            rsprintf("    if (document.form1.d%d.value == \"\") {\n", i);
            sprintf(str, loc("Please enter day for attribute '%s'"), attr_list[i]);
            rsprintf("      alert(\"%s\");\n", str);
            rsprintf("      document.form1.d%d.focus();\n", i);
            rsprintf("      return false;\n");
            rsprintf("    }\n");
            rsprintf("    if (document.form1.y%d.value == \"\") {\n", i);
            sprintf(str, loc("Please enter year for attribute '%s'"), attr_list[i]);
            rsprintf("      alert(\"%s\");\n", str);
            rsprintf("      document.form1.y%d.focus();\n", i);
            rsprintf("      return false;\n");
            rsprintf("    }\n");

            if (attr_flags[i] & AF_DATETIME) {
               rsprintf("    if (document.form1.h%d.value == \"\") {\n", i);
               sprintf(str, loc("Please enter hour for attribute '%s'"), attr_list[i]);
               rsprintf("      alert(\"%s\");\n", str);
               rsprintf("      document.form1.h%d.focus();\n", i);
               rsprintf("      return false;\n");
               rsprintf("    }\n");
               rsprintf("    if (document.form1.n%d.value == \"\") {\n", i);
               sprintf(str, loc("Please enter minute for attribute '%s'"), attr_list[i]);
               rsprintf("      alert(\"%s\");\n", str);
               rsprintf("      document.form1.n%d.focus();\n", i);
               rsprintf("      return false;\n");
               rsprintf("    }\n");
               rsprintf("    if (document.form1.c%d.value == \"\") {\n", i);
               sprintf(str, loc("Please enter second for attribute '%s'"), attr_list[i]);
               rsprintf("      alert(\"%s\");\n", str);
               rsprintf("      document.form1.c%d.focus();\n", i);
               rsprintf("      return false;\n");
               rsprintf("    }\n");
            }

         } else {
            rsprintf("    if (document.form1.%s.value == \"\") {\n", ua);
            sprintf(str, loc("Please enter attribute '%s'"), attr_list[i]);
            rsprintf("      alert(\"%s\");\n", str);
            rsprintf("      document.form1.%s.focus();\n", ua);
            rsprintf("      return false;\n");
            rsprintf("    }\n");
         }

         rsprintf("  }\n");
         rsprintf("  catch(err) {\n");
         rsprintf("  }\n\n");
      }

      if ((attr_flags[i] & AF_NUMERIC) && !(attr_flags[i] & AF_LOCKED)) {
         /* convert blanks etc. to underscores */
         strcpy(ua, attr_list[i]);
         stou(ua);

         rsprintf("  try {\n"); // try-catch block in case attributes are not there (conditional)
         rsprintf("    if (document.form1.%s.value != \"- %s -\") {\n", ua, loc("keep original values"));
         rsprintf("      for (var i=0 ; i<document.form1.%s.value.length ; i++)\n", ua);
         rsprintf("        if (document.form1.%s.value.charAt(i) != \",\" &&\n", ua);
         rsprintf("            document.form1.%s.value.charAt(i) != \".\" &&\n", ua);
         rsprintf("            document.form1.%s.value.charAt(i) != \"-\" &&\n", ua);
         rsprintf("            (document.form1.%s.value.charAt(i) < \"0\" ||\n", ua);
         rsprintf("             document.form1.%s.value.charAt(i) > \"9\")) { break }\n", ua);
         rsprintf("      if (i<document.form1.%s.value.length) {\n", ua);
         sprintf(str, loc("Please enter numeric value for '%s'"), attr_list[i]);
         rsprintf("        alert(\"%s\");\n", str);
         rsprintf("        document.form1.%s.focus();\n", ua);
         rsprintf("        return false;\n");
         rsprintf("      }\n");
         rsprintf("    }\n");
         rsprintf("  }\n");
         rsprintf("  catch(err) {\n");
         rsprintf("  }\n\n");
      }

      if ((attr_flags[i] & (AF_DATE | AF_DATETIME)) && !(attr_flags[i] & AF_LOCKED)) {
         rsprintf("  try {\n"); // try-catch block in case attributes are not there (conditional)
         rsprintf("    for (var i=0 ; i<document.form1.y%d.value.length ; i++)\n", i);
         rsprintf("      if ((document.form1.y%d.value.charAt(i) < \"0\" ||\n", i);
         rsprintf("           document.form1.y%d.value.charAt(i) > \"9\")) { break }\n", i);
         rsprintf("    if (i<document.form1.y%d.value.length) {\n", i);
         sprintf(str, loc("Please enter numeric value for year of attribute '%s'"), attr_list[i]);
         rsprintf("      alert(\"%s\");\n", str);
         rsprintf("      document.form1.y%d.focus();\n", i);
         rsprintf("      return false;\n");
         rsprintf("    }\n");
         rsprintf("  }\n");
         rsprintf("  catch(err) {\n");
         rsprintf("  }\n\n");
      }
   }

   rsprintf("  if (autoSaveTimer != null)\n");
   rsprintf("    clearTimeout(autoSaveTimer);\n");
   rsprintf("  button.disabled = true;\n");
   rsprintf("  button.value = \"%s...\";\n", loc("Please wait"));
   rsprintf("  submitted = true;\n");
   rsprintf("  document.getElementById(\"form1\").elements['cmd'][0].value = \"%s\";\n", loc("Submit"));
   rsprintf("  document.getElementById(\"form1\").submit();\n");
   rsprintf("  return true;\n");
   rsprintf("}\n\n");

   /* mark_submitted() gets called via "Back" and "Preview" buttons */
   rsprintf("function mark_submitted()\n");
   rsprintf("{\n");
   rsprintf("  if (autoSaveTimer != null)\n");
   rsprintf("    clearTimeout(autoSaveTimer);\n");
   rsprintf("  submitted = true;\n"); // don't ask to leave that page
   rsprintf("  return true;\n");
   rsprintf("}\n\n");

   /* check_delte() gets called via "Delete" button */
   rsprintf("function check_delete()\n");
   rsprintf("{\n");
   rsprintf("  var ret = confirm('%s');\n", loc("Really delete this entry?"));
   rsprintf("  if (ret) {\n");
   rsprintf("    mark_submitted()\n");
   rsprintf("    document.form1.jcmd.value='XDelete';\n");
   rsprintf("    return true;\n");
   rsprintf("  }\n");
   rsprintf("  return false;\n");
   rsprintf("}\n\n");

   /* chkupload() gets called via "Upload" button */
   rsprintf("function chkupload()\n");
   rsprintf("{\n");
   rsprintf("  if (document.form1.attfile.value == \"\") {\n");
   rsprintf("    alert(\"%s\");\n", loc("No attachment file specified"));
   rsprintf("    return false;\n");
   rsprintf("  }\n");
   rsprintf("  mark_submitted();\n");
   rsprintf("  return true;\n");
   rsprintf("}\n\n");

   /* cond_submit() gets called via selection of new conditional attribute */
   rsprintf("function cond_submit()\n");
   rsprintf("{\n");
   rsprintf("  submitted = true;\n");
   rsprintf("  document.form1.submit();\n");
   rsprintf("}\n\n");

   /* save_draft() gets called via the "Save" button */
   rsprintf("function save_draft()\n");
   rsprintf("{\n");
   rsprintf("  if (autoSaveTimer != null)\n");
   rsprintf("    clearTimeout(autoSaveTimer);\n");
   rsprintf("  asend();\n");
   rsprintf("  draft_modified = false;\n");
   rsprintf("  return false;\n");
   rsprintf("}\n\n");

   /* beforeunload() gets called "onBeforeUnload" */
   rsprintf("function beforeunload(e)\n");
   rsprintf("{\n");
   rsprintf("  if (!submitted)\n");
   rsprintf("    e.returnValue = \"%s\";\n", loc("If you leave this page you will lose your unsaved changes"));
   rsprintf("}\n\n");

   /* unload() gets called "onUnload", issues a "Unlock" command to remove a possible lock */
   rsprintf("function unload()\n");
   rsprintf("{\n");
   rsprintf("  if (!submitted) {\n");
   rsprintf("    r = XMLHttpRequestGeneric();\n");
   rsprintf("    r.open('GET', '?jcmd=Unlock&edit_id=%d', true);\n", message_id);
   rsprintf("    r.send();\n");
   rsprintf("  }\n");
   rsprintf("}\n\n");

   /* mod() gets called via "onchange" event */
   rsprintf("var autoSaveTimer;\n");
   rsprintf("var checkTextTimer;\n");
   rsprintf("var oldText;\n\n");
   rsprintf("var initialText;\n\n");

   if (getcfg(lbs->name, "Autosave", str, sizeof(str)))
      autosave = atoi(str);
   else
      autosave = 10;

   if (getcfg(lbs->name, "Save drafts", str, sizeof(str)) && atoi(str) == 0)
      autosave = 0;

   rsprintf("function mod(e)\n");
   rsprintf("{\n");
   if (autosave) {
      rsprintf("  if (!draft_modified)\n");
      rsprintf("    autoSaveTimer = setTimeout(save_draft, %d);\n", autosave * 1000);
   }
   rsprintf("  entry_modified = true;\n");
   rsprintf("  draft_modified = true;\n");
   rsprintf("  window.status = \"%s\";\n", loc("Entry has been modified"));
   rsprintf("  document.form1.entry_modified.value = \"1\";\n");
   rsprintf("  document.title = '%s - %s';\n", page_title, loc("Edited"));
   rsprintf("  if (document.getElementById('restore') != undefined)\n");
   rsprintf("     document.getElementById('restore').disabled = false;\n");
   rsprintf("}\n\n");

   rsprintf("function checkText()\n");
   rsprintf("{\n");
   if (autosave) {
      // CKEDITOR cannot call mod(), so manually check if text has changed
      rsprintf("  if (checkTextTimer == null) {\n");
      rsprintf("    checkTextTimer = setTimeout(checkText, 1000);\n");
      rsprintf("    return;\n");
      rsprintf("  }\n");
      rsprintf("  clearTimeout(checkTextTimer);\n");
      rsprintf("  if (typeof(CKEDITOR) != 'undefined')\n");
      rsprintf("    t = CKEDITOR.instances.Text.getData();\n");
      rsprintf("   else\n");
      rsprintf("    t = document.form1.Text.value;\n");
      rsprintf("  if (oldText == null)\n");
      rsprintf("    oldText = t;\n");
      rsprintf("  if (initialText == null)\n");
      rsprintf("    initialText = t;\n");
      rsprintf("  if (oldText != t)\n");
      rsprintf("    mod();\n");
      rsprintf("  oldText = t;\n");
      rsprintf("  checkTextTimer = setTimeout(checkText, 1000);\n");
   }
   rsprintf("}\n\n");

   rsprintf("function restoreText()\n");
   rsprintf("{\n");
   rsprintf("  if (initialText != null) {\n");
   rsprintf("    if (confirm('%s'+'\\n'+'%s')) {\n", loc("Do you want to restore your original text?"),
            loc("This will overwrite your current modifications."));
   rsprintf("      if (typeof(CKEDITOR) != 'undefined')\n");
   rsprintf("        CKEDITOR.instances.Text.setData(initialText);\n");
   rsprintf("      else\n");
   rsprintf("        document.form1.Text.value = initialText;\n");
   rsprintf("    }\n");
   rsprintf("  }\n");
   rsprintf("}\n\n");

   rsprintf("function kp(e)\n");
   rsprintf("{\n");
   rsprintf("  last_key = (e.which) ? e.which : event.keyCode;\n");
   rsprintf("  mod();\n");
   rsprintf("}\n\n");

   /* switch_smileys turn on/off the smiley bar */
   rsprintf("function switch_smileys()\n");
   rsprintf("{\n");
   rsprintf("   s = document.getElementById('smileyRow');\n");
   rsprintf("   i = document.getElementById('smileyIcon');\n");
   rsprintf("   if (s.style.display == 'none') {\n");
   rsprintf("     s.style.display = 'table-row';\n");
   rsprintf("     i.src = 'icons/eld_smile.png';\n");
   rsprintf("   } else {\n");
   rsprintf("     s.style.display = 'none';\n");
   rsprintf("     i.src = 'icons/elc_smile.png';\n");
   rsprintf("   }\n");
   rsprintf("}\n\n");

   if (/*enc_selected != 2 && */ !getcfg(lbs->name, "Message height", str, sizeof(str)) &&
                                 !getcfg(lbs->name, "Message width", str, sizeof(str))) {
      /* javascript for resizing edit box */
      rsprintf("function init_resize()\n");
      rsprintf("{\n");
      rsprintf("   window.onresize = resize_textarea;\n");
      rsprintf("   resize_textarea();\n");
      rsprintf("}\n\n");
      rsprintf("function resize_textarea()\n");
      rsprintf("{\n");
      rsprintf("   p = $id('TextParent');\n");
      rsprintf("   t = p.getBoundingClientRect().top;\n");
      rsprintf("   if (!!document.documentMode) // IE\n");
      rsprintf("      height = window.innerHeight - t - 210;\n");
      rsprintf("   else\n");
      rsprintf("      height = window.innerHeight - t - 205;\n");
      rsprintf("   if (height < 300)\n");
      rsprintf("      height = 300;\n");
      rsprintf("   width = window.innerWidth;\n");
      rsprintf("   if (width < 300)\n");
      rsprintf("      width = 300;\n");
      rsprintf("   width = width - 8;\n");
      rsprintf("   if (typeof(CKEDITOR) != 'undefined')\n");
      rsprintf("      CKEDITOR.instances.Text.resize(width, height);\n");
      rsprintf("   else {\n");
      rsprintf("      document.form1.Text.style.height = height+6+\"px\";\n");
      rsprintf("      document.form1.Text.style.width = width-6+\"px\";\n");
      rsprintf("   }\n");
      rsprintf("}\n\n");
   }

   /* ToggleAll() to toggle MOptions buttons */
   rsprintf("function ToggleAll(attrib)\n");
   rsprintf("{\n");
   rsprintf("  for (var i = 0; i < document.form1.elements.length; i++) {\n");
   rsprintf(
           "    if (document.form1.elements[i].type == 'checkbox' && document.form1.elements[i].disabled == false) {\n");
   rsprintf("      a = document.form1.elements[i].name;\n");
   rsprintf("      a = a.substring(0, attrib.length);\n");
   rsprintf("      if (a == attrib)\n");
   rsprintf("        document.form1.elements[i].checked = !(document.form1.elements[i].checked);\n");
   rsprintf("    }\n");
   rsprintf("  }\n");
   rsprintf("}\n\n");

   /* language for CKEDITOR */
   if (getcfg("global", "language", str, sizeof(str))) {
      for (i = 0; lang_table[i].language[0]; i++) {
         strlcpy(str2, str, sizeof(str2));
         str2[strlen(lang_table[i].language)] = 0;
         if (stricmp(str2, lang_table[i].language) == 0)
            break;
      }
      if (lang_table[i].language[0])
         rsprintf("var CKEditorLang = '%s';\n", lang_table[i].abbrev);
      else
         rsprintf("var CKEditorLang = 'en';\n");
   } else
      rsprintf("var CKEditorLang = 'en';\n");

   /* strings for elcode.js */
   if (enc_selected == 0) {
      rsprintf("var linkText_prompt = \"%s\";\n", loc("Enter name of hyperlink"));
      rsprintf("var linkURL_prompt  = \"%s\";\n", loc("Enter URL of hyperlink"));
      rsprintf("var linkHeading_prompt  = \"%s\";\n", loc("Enter heading level (1, 2 or 3)"));
   }

   show_browser(browser);

   rsprintf("var logbook = \"%s\";\n", lbs->name_enc);
   for (i = 0; i < MAX_ATTACHMENTS; i++)
      if (!att[i][0]) {
         /* put first free attachment for uploader */
         rsprintf("var next_attachment = %d;\n", i + 1);
         break;
      }
   rsprintf("var page_title = '%s';\n", page_title);

   rsprintf("\n");
   rsprintf("window.onbeforeunload = beforeunload;\n");
   rsprintf("\n");

   rsprintf("//-->\n");
   rsprintf("</script>\n");

   /* optionally load ImageMagic JavaScript code */
   if (image_magick_exist)
      rsprintf("<script type=\"text/javascript\" src=\"../im.js\"></script>\n\n");

   /* optionally load ELCode JavaScript code */
   if (enc_selected == 0)
      rsprintf("<script type=\"text/javascript\" src=\"../elcode.js\"></script>\n\n");

   show_text = !getcfg(lbs->name, "Show text", str, sizeof(str)) || atoi(str) == 1;
   fixed_text = getcfg(lbs->name, "Fix text", str, sizeof(str)) && atoi(str) == 1 && bedit && message_id;

   if (enc_selected == 2 && ckedit_exist && show_text && !fixed_text) {
      rsprintf("<script type=\"text/javascript\" src=\"../ckeditor/ckeditor.js\"></script>\n");
   }

   rsprintf("<script type=\"text/javascript\" src=\"../jquery-1.11.1.min.js\"></script>\n");
   rsprintf("<script type=\"text/javascript\" src=\"../progress/progress.min.js\"></script>\n");
   rsprintf("<link rel=\"stylesheet\" type=\"text/css\" href=\"../progress/progressjs.min.css\">\n");

   /* drag-and-drip script */
   rsprintf("<script type=\"text/javascript\" src=\"../dnd.js\"></script>\n");

   /* CKEDITOR */
   if (enc_selected == 2 && ckedit_exist && show_text && !fixed_text)
      rsprintf("<script type=\"text/javascript\" src=\"../load-ckeditor.js\"></script>\n");

   /* external script if requested */
   if (isparam("js")) {
      rsprintf("<script src=\"%s\" type=\"text/javascript\">\n", getparam("js"));
      rsprintf("</script>\n\n");
   }

   script_onload[0] = 0;
   script_onfocus[0] = 0;
   if ((isparam("inlineatt") && *getparam("inlineatt")) || bpreview)
      strcpy(script_onload, "document.form1.Text.focus();");
   else
      strcpy(script_onload, "i=document.getElementById('fid');if(i)i.focus();");

   if (enc_selected == 0) {
      if (!getcfg(lbs->name, "Message height", str, sizeof(str)) &&
          !getcfg(lbs->name, "Message width", str, sizeof(str))) {
         strcat(script_onload, "elKeyInit();init_resize();");
         strcat(script_onfocus, "elKeyInit();");
      } else
         strcat(script_onload, "elKeyInit();");
      strcat(script_onfocus, "elKeyInit();");
   } else if (enc_selected == 1) {
      if (!getcfg(lbs->name, "Message height", str, sizeof(str)) &&
          !getcfg(lbs->name, "Message width", str, sizeof(str)))
         strcat(script_onload, "init_resize();");
   }
   strcat(script_onload, "checkText();dndInit();");

   script_onunload[0] = 0;
   if (getcfg(lbs->name, "Use Lock", str, sizeof(str)) && atoi(str) == 1)
      strcat(script_onunload, "unload();");

   rsprintf("</head>\n\n");
   rsprintf("<body");

   if (script_onload[0])
      rsprintf(" OnLoad=\"%s\"", script_onload);
   if (script_onfocus[0])
      rsprintf(" OnFocus=\"%s\"", script_onfocus);
   if (script_onunload[0])
      rsprintf(" OnUnload=\"%s\"", script_onunload);
   rsprintf(">\n");

   show_top_text(lbs);
   rsprintf("<form name=\"form1\" id=\"form1\" method=\"POST\" action=\"./\" ");
   rsprintf("enctype=\"multipart/form-data\">\n");

   /*---- add password in case cookie expires during edit ----*/

   if (getcfg(lbs->name, "Password file", str, sizeof(str)) && isparam("unm")) {
      strencode2(str, getparam("unm"), sizeof(str));
      rsprintf("<input type=hidden name=\"unm\" value=\"%s\">\n", str);
      if (isparam("upwd"))
         strlcpy(upwd, getparam("upwd"), sizeof(upwd));
      else
         get_user_line(lbs, getparam("unm"), upwd, NULL, NULL, NULL, NULL, NULL);
      rsprintf("<input type=hidden name=\"upwd\" value=\"%s\">\n", upwd);
   }

   rsprintf("<input type=hidden name=\"jcmd\">\n");
   rsprintf("<input type=hidden name=\"smcmd\">\n");
   rsprintf("<input type=hidden name=\"inlineatt\">\n");

   if (new_entry)
      rsprintf("<input type=hidden name=\"new_entry\" value=\"1\">\n");

   if (isparam("entry_modified") && atoi(getparam("entry_modified")) == 1)
      rsprintf("<input type=hidden name=\"entry_modified\" value=\"1\">\n");
   else
      rsprintf("<input type=hidden name=\"entry_modified\" value=\"0\">\n");

   /*---- title row ----*/

   show_standard_title(lbs, "", 0);

   /*---- menu buttons ----*/

   rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");

   /* default cmd */
   rsprintf("<input type=hidden name=cmd value=\"%s\">\n", loc("Update"));

   rsprintf("<input type=\"submit\" name=\"cmd\" value=\"%s\" onClick=\"return chkform(this);\">\n",
            loc("Submit"));

   if (!getcfg(lbs->name, "Save drafts", str, sizeof(str)) || atoi(str) == 1)
      rsprintf("<input type=\"submit\" name=\"cmd\" value=\"%s\" onClick=\"return save_draft();\">\n",
               loc("Save"));

   if (!getcfg(lbs->name, "Show text", str, sizeof(str)) || atoi(str) == 1)
      rsprintf("<input type=\"submit\" name=\"cmd\" value=\"%s\" onClick=\"return mark_submitted();\">\n",
               loc("Preview"));

   if (message_id && (!getcfg(lbs->name, "Show text", str, sizeof(str)) || atoi(str) == 1))
      rsprintf(
              "<input type=\"submit\" name=\"cmd\" id=\"restore\" value=\"%s\" disabled onClick=\"restoreText();return false;\">\n",
              loc("Restore"));

   if (!getcfg(lbs->name, "Save drafts", str, sizeof(str)) || atoi(str) == 1)
      rsprintf("<input type=\"submit\" name=\"cmd\" value=\"%s\" onClick=\"return check_delete();\">\n",
               loc("Delete"));
   else
      rsprintf("<input type=\"submit\" name=\"cmd\" value=\"%s\" onClick=\"return mark_submitted();\">\n",
               loc("Back"));

   rsprintf(
           "&nbsp;&nbsp;<span id=\"saved1\" style=\"font-size:10px;font-style:italic;display:none\">%s 00:00:00</span>",
           loc("Draft saved at"));

   rsprintf("</span></td></tr>\n\n");

   /*---- entry form ----*/

   /* table for two-column items */
   rsprintf("<tr><td><table class=\"listframe\" width=\"100%%\" cellspacing=\"0\" cellpadding=\"0\">");

   /* print required message if one of the attributes has it set */
   for (i = 0; i < n_attr; i++) {
      if (attr_flags[i] & AF_REQUIRED) {
         rsprintf("<tr><td colspan=2 class=\"attribvalue\">%s <font color=red>*</font> %s</td></tr>\n",
                  loc("Fields marked with"), loc("are required"));
         break;
      }
   }

   if (!isparam("nsel")) {
      time(&now);
      if (bedit && date[0]) {
         if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
            strcpy(format, DEFAULT_TIME_FORMAT);

         ltime = date_to_ltime(date);
         pts = localtime(&ltime);
         assert(pts);
         my_strftime(str, sizeof(str), format, pts);
      } else {
         if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
            strcpy(format, DEFAULT_TIME_FORMAT);
         my_strftime(str, sizeof(str), format, localtime(&now));
         strcpy(date, ctime(&now));
         date[24] = 0;
      }

      rsprintf("<tr><td nowrap width=\"10%%\" class=\"attribname\">%s:</td>", loc("Entry time"));
      rsprintf("<td class=\"attribvalue\">%s\n", str);

      strencode2(str, date, sizeof(str));
      rsprintf("<input type=hidden name=entry_date value=\"%s\"></td></tr>\n", str);
   }

   if (_condition[0])
      rsprintf("<input type=hidden name=condition value=\"%s\"></td></tr>\n", _condition);

   /* retrieve attribute flags */
   for (i = 0; i < n_attr; i++) {
      format_flags[i] = 0;
      sprintf(str, "Format %s", attr_list[i]);
      if (getcfg(lbs->name, str, format, sizeof(format))) {
         n = strbreak(format, fl, 8, ",", FALSE);
         if (n > 0)
            format_flags[i] = atoi(fl[0]);
      }
   }

   subtable = 0;

   /* generate list of attributes to show */
   if (getcfg(lbs->name, "Show attributes edit", str, sizeof(str))) {
      n_disp_attr = strbreak(str, list, MAX_N_ATTR, ",", FALSE);
      for (i = 0; i < n_disp_attr; i++) {
         for (j = 0; j < n_attr; j++)
            if (strieq(attr_list[j], list[i]))
               break;
         if (!strieq(attr_list[j], list[i]))
            /* attribute not found */
            j = 0;
         attr_index[i] = j;
      }
   } else {
      for (i = 0; i < n_attr; i++)
         attr_index[i] = i;
      n_disp_attr = n_attr;
   }

   strcpy(fid, "id=\"fid\" ");

   /* display attributes */
   for (aindex = 0; aindex < n_disp_attr; aindex++) {

      index = attr_index[aindex];

      strcpy(class_name, "attribname");
      strcpy(class_value, "attribvalue");
      input_size = 80;
      input_maxlen = NAME_LENGTH;
      strcpy(ua, attr_list[index]);
      stou(ua);

      sprintf(str, "Format %s", attr_list[index]);
      if (getcfg(lbs->name, str, format, sizeof(format))) {
         n = strbreak(format, fl, 8, ",", FALSE);
         if (n > 1)
            strlcpy(class_name, fl[1], sizeof(class_name));
         if (n > 2)
            strlcpy(class_value, fl[2], sizeof(class_value));
         if (n > 3 && atoi(fl[3]) > 0)
            input_size = atoi(fl[3]);
         if (n > 4 && atoi(fl[4]) > 0)
            input_maxlen = atoi(fl[4]);
      }

      if (format_flags[index] & AFF_SAME_LINE)
         /* if attribute on same line, do nothing */
         rsprintf("");
      else if (aindex < n_disp_attr - 1 && (format_flags[attr_index[aindex + 1]] & AFF_SAME_LINE)) {
         /* if next attribute on same line, start a new subtable */
         rsprintf("<tr><td colspan=2><table width=\"100%%\" cellpadding=\"0\" cellspacing=\"0\"><tr>");
         subtable = 1;
      } else
         /* for normal attribute, start new row */
         rsprintf("<tr>");

      strcpy(star, (attr_flags[index] & AF_REQUIRED) ? "<font color=red>*</font>" : "");

      /* display text box with optional tooltip */
      sprintf(str, "Tooltip %s", attr_list[index]);
      title[0] = 0;
      if (getcfg(lbs->name, str, comment, sizeof(comment)))
         sprintf(title, " title=\"%s\"", comment);

      rsprintf("<td%s nowrap class=\"%s\">", title, class_name);

      /* display attribute name */
      rsprintf("%s%s:", attr_list[index], star);

      /* show optional comment */
      sprintf(str, "Comment %s", attr_list[index]);
      if (getcfg(lbs->name, str, comment, sizeof(comment)))
         rsprintf("<br><span class=\"selcomment\"><b>%s</b></span>\n", comment);

      rsprintf("</td>\n");

      /* if attribute cannot be changed, just display text */
      if ((attr_flags[index] & AF_LOCKED) ||
          (message_id && bedit && (attr_flags[index] & AF_FIXED_EDIT)) ||
          (message_id && !bedit && (attr_flags[index] & AF_FIXED_REPLY))) {

         if (attr_flags[index] & AF_DATE) {

            sprintf(str, "Date format %s", attr_list[i]);
            if (!getcfg(lbs->name, str, format, sizeof(format)))
               if (!getcfg(lbs->name, "Date format", format, sizeof(format)))
                  strcpy(format, DEFAULT_DATE_FORMAT);

            ltime = atoi(attrib[index]);
            pts = localtime(&ltime);
            assert(pts);
            if (ltime == 0)
               strcpy(str, "-");
            else
               my_strftime(str, sizeof(str), format, pts);

         } else if (attr_flags[index] & AF_DATETIME) {

            sprintf(str, "Time format %s", attr_list[i]);
            if (!getcfg(lbs->name, str, format, sizeof(format)))
               if (!getcfg(lbs->name, "Time format", format, sizeof(format)))
                  strcpy(format, DEFAULT_TIME_FORMAT);

            ltime = atoi(attrib[index]);
            pts = localtime(&ltime);
            assert(pts);
            if (ltime == 0)
               strcpy(str, "-");
            else
               my_strftime(str, sizeof(str), format, pts);

         } else
            strlcpy(str, attrib[index], sizeof(str));

         rsprintf("<td%s class=\"%s\">\n", title, class_value);
         rsputs2(lbs, FALSE, str);
         rsprintf("&nbsp;");

         if (attr_flags[index] & AF_MULTI) {
            for (i = 0; i < MAX_N_LIST && attr_options[index][i][0]; i++) {
               sprintf(str, "%s_%d", ua, i);

               if (strstr(attrib[index], attr_options[index][i]))
                  rsprintf("<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", str, attr_options[index][i]);
            }
         } else if (attr_flags[index] & AF_MUSERLIST) {

            for (i = 0;; i++) {
               if (!enum_user_line(lbs, i, login_name, sizeof(login_name)))
                  break;
               get_user_line(lbs, login_name, NULL, full_name, NULL, NULL, NULL, NULL);

               sprintf(str, "%s_%d", ua, i);

               if (strstr(attrib[index], full_name))
                  rsprintf("<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", str, full_name);
            }
         } else if (attr_flags[index] & AF_MUSEREMAIL) {

            for (i = 0;; i++) {
               if (!enum_user_line(lbs, i, login_name, sizeof(login_name)))
                  break;
               get_user_line(lbs, login_name, NULL, NULL, user_email, NULL, NULL, NULL);

               sprintf(str, "%s_%d", ua, i);

               if (strstr(attrib[index], user_email))
                  rsprintf("<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", str, user_email);
            }
         } else if (attr_flags[index] & AF_ICON) {
            for (i = 0; i < MAX_N_LIST && attr_options[index][i][0]; i++) {
               sprintf(str, "%s_%d", ua, i);

               if (strstr(attrib[index], attr_options[index][i]))
                  rsprintf("<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", str, attr_options[index][i]);
            }
         } else {
            strencode2(str, attrib[index], sizeof(str));
            rsprintf("<input type=\"hidden\" name=\"%s\" value=\"%s\"></td>\n", ua, str);
         }
      } else {
         if (attr_options[index][0][0] == 0) {

            if (attr_flags[index] & AF_DATE) {

               year = month = day = 0;
               if (attrib[index][0]) {
                  ltime = atoi(attrib[index]);
                  if (ltime > 0) {
                     pts = localtime(&ltime);
                     assert(pts);
                     year = pts->tm_year + 1900;
                     month = pts->tm_mon + 1;
                     day = pts->tm_mday;
                  }
               }

               rsprintf("<td%s class=\"attribvalue\">", title);
               sprintf(str, "%d", index);
               show_date_selector(day, month, year, str);
               rsprintf("</td>\n");

            } else if (attr_flags[index] & AF_DATETIME) {

               year = month = day = 0;
               hour = min = sec = -1;
               if (attrib[index][0]) {
                  ltime = atoi(attrib[index]);
                  if (ltime > 0) {
                     pts = localtime(&ltime);
                     assert(pts);
                     year = pts->tm_year + 1900;
                     month = pts->tm_mon + 1;
                     day = pts->tm_mday;
                     hour = pts->tm_hour;
                     min = pts->tm_min;
                     sec = pts->tm_sec;
                  }
               }

               rsprintf("<td%s class=\"%s\">", title, class_value);
               sprintf(str, "%d", index);
               show_date_selector(day, month, year, str);
               rsprintf("&nbsp;&nbsp;");
               show_time_selector(hour, min, sec, str);

               rsprintf("</td>\n");

            } else if (attr_flags[index] & AF_USERLIST) {

               rsprintf("<td%s class=\"%s\">\n", title, class_value);

               /* display drop-down box with list of users */
               rsprintf("<select name=\"%s\"", ua);
               rsprintf(" onChange=\"mod();\">\n");

               /* display emtpy option */
               sprintf(str, "- %s -", loc("keep original values"));
               if (strcmp(str, attrib[index]) == 0 && isparam("nsel"))
                  rsprintf("<option value=\"<keep>\">%s\n", str);
               else
                  rsprintf("<option value=\"\">- %s -\n", loc("please select"));

               for (i = 0;; i++) {
                  if (!enum_user_line(lbs, i, login_name, sizeof(login_name)))
                     break;
                  get_user_line(lbs, login_name, NULL, str, NULL, NULL, NULL, NULL);

                  if (strieq(str, attrib[index]))
                     rsprintf("<option selected value=\"%s\">%s\n", str, str);
                  else
                     rsprintf("<option value=\"%s\">%s\n", str, str);
               }

               rsprintf("</select>\n");

               rsprintf("</td>\n");

            } else if (attr_flags[index] & AF_MUSERLIST) {

               /* display multiple check boxes with user names */
               rsprintf("<td%s class=\"%s\">\n", title, class_value);

               n_moptions = strbreak(attrib[index], attr_moptions, MAX_N_LIST, "|", FALSE);

               /* allocate list of users and populate it */
               for (n = 0;; n++) {
                  if (!enum_user_line(lbs, n, login_name, sizeof(login_name)))
                     break;
               }

               user_list = xcalloc(sizeof(char *), n);
               for (i = 0; i < n; i++)
                  user_list[i] = xcalloc(NAME_LENGTH, 1);

               for (i = 0; i < n; i++) {
                  enum_user_line(lbs, i, str, NAME_LENGTH);
                  get_user_line(lbs, str, NULL, user_list[i], NULL, NULL, NULL, NULL);
               }

               /* sort list */
               qsort(user_list, n, sizeof(char *), ascii_compare);

               for (i = 0; i < n; i++) {
                  sprintf(str, "%s_%d", ua, i);

                  rsprintf("<span style=\"white-space:nowrap;\">\n");

                  for (j = 0; j < n_moptions; j++)
                     if (strcmp(attr_moptions[j], user_list[i]) == 0)
                        break;

                  if (j < n_moptions)
                     rsprintf
                             ("<input type=checkbox id=\"%s\" name=\"%s\" value=\"%s\" checked onChange=\"mod();\">\n",
                              str, str, user_list[i]);
                  else
                     rsprintf
                             ("<input type=checkbox id=\"%s\" name=\"%s\" value=\"%s\" onChange=\"mod();\">\n",
                              str, str, user_list[i]);

                  rsprintf("<label for=\"%s\">%s</label>\n", str, user_list[i]);
                  rsprintf("</span>\n");

                  if (format_flags[index] & AFF_MULTI_LINE)
                     rsprintf("<br>");
               }

               rsprintf("</td>\n");

               for (i = 0; i < n; i++)
                  xfree(user_list[i]);
               xfree(user_list);

            } else if (attr_flags[index] & AF_MUSEREMAIL) {

               /* display multiple check boxes with user emails */
               rsprintf("<td%s class=\"%s\">\n", title, class_value);

               n_moptions = strbreak(attrib[index], attr_moptions, MAX_N_LIST, "|", FALSE);

               /* allocate list of users and populate it */
               for (n = 0;; n++) {
                  if (!enum_user_line(lbs, n, login_name, sizeof(login_name)))
                     break;
               }

               user_list = xcalloc(sizeof(char *), n);
               for (i = 0; i < n; i++)
                  user_list[i] = xcalloc(NAME_LENGTH, 1);

               for (i = 0; i < n; i++) {
                  enum_user_line(lbs, i, str, NAME_LENGTH);
                  get_user_line(lbs, str, NULL, NULL, user_list[i], NULL, NULL, NULL);
               }

               /* sort list */
               qsort(user_list, n, sizeof(char *), ascii_compare);

               for (i = 0; i < n; i++) {
                  sprintf(str, "%s_%d", ua, i);

                  rsprintf("<span style=\"white-space:nowrap;\">\n");

                  for (j = 0; j < n_moptions; j++)
                     if (strcmp(attr_moptions[j], user_list[i]) == 0 ||
                         strcmp(attr_moptions[j] + 7, user_list[i]) == 0)
                        break;

                  if (j < n_moptions)
                     rsprintf
                             ("<input type=checkbox id=\"%s\" name=\"%s\" value=\"%s\" checked onChange=\"mod();\">\n",
                              str, str, user_list[i]);
                  else
                     rsprintf
                             ("<input type=checkbox id=\"%s\" name=\"%s\" value=\"%s\" onChange=\"mod();\">\n",
                              str, str, user_list[i]);

                  rsprintf("<label for=\"%s\">%s</label>\n", str, user_list[i]);
                  rsprintf("</span>\n");

                  if (format_flags[index] & AFF_MULTI_LINE)
                     rsprintf("<br>");
               }

               rsprintf("</td>\n");

               for (i = 0; i < n; i++)
                  xfree(user_list[i]);
               xfree(user_list);

            } else if (attr_flags[index] & AF_USEREMAIL) {

               rsprintf("<td%s class=\"%s\">\n", title, class_value);

               /* display drop-down box with list of users */
               rsprintf("<select name=\"%s\"", ua);
               rsprintf(" onChange=\"mod();\">\n");

               /* display emtpy option */
               sprintf(str, "- %s -", loc("keep original values"));
               if (strcmp(str, attrib[index]) == 0 && isparam("nsel"))
                  rsprintf("<option value=\"<keep>\">%s\n", str);
               else
                  rsprintf("<option value=\"\">- %s -\n", loc("please select"));

               for (i = 0;; i++) {
                  if (!enum_user_line(lbs, i, login_name, sizeof(login_name)))
                     break;
                  get_user_line(lbs, login_name, NULL, NULL, str, NULL, NULL, NULL);

                  if (strieq(str, attrib[index]) || strieq(str, attrib[index] + 7))
                     rsprintf("<option selected value=\"%s\">%s\n", str, str);
                  else
                     rsprintf("<option value=\"%s\">%s\n", str, str);
               }

               rsprintf("</select>\n");

               rsprintf("</td>\n");

            } else {

               /* show normal edit field */
               rsprintf("<td%s class=\"%s\">", title, class_value);

               strencode2(str, attrib[index], sizeof(str));
               rsprintf
                       ("<input type=\"text\" %ssize=%d maxlength=%d name=\"%s\" value=\"%s\" onKeyPress=\"kp(event)\" onChange=\"mod()\">\n",
                        fid, input_size, input_maxlen, ua, str);
               fid[0] = 0;

               rsprintf("</td>\n");
            }

         } else {
            if (strieq(attr_options[index][0], "boolean")) {
               /* display three radio buttons instead of three-state check box for multi-edit */
               sprintf(str, "- %s -", loc("keep original values"));
               if (isparam("nsel") && strieq(attrib[index], str)) {
                  rsprintf("<td%s class=\"%s\">", title, class_value);
                  sprintf(str, "%s_0", ua);
                  rsprintf("<span style=\"white-space:nowrap;\">\n");
                  rsprintf("<input type=radio id=\"%s\" name=\"%s\" value=\"0\" onChange=\"mod();\">\n", str,
                           ua);
                  rsprintf("<label for=\"%s\">0</label>\n", str);
                  rsprintf("</span>\n");

                  sprintf(str, "%s_1", ua);
                  rsprintf("<span style=\"white-space:nowrap;\">\n");
                  rsprintf("<input type=radio id=\"%s\" name=\"%s\" value=\"1\" onChange=\"mod();\">\n", str,
                           ua);
                  rsprintf("<label for=\"%s\">1</label>\n", str);
                  rsprintf("</span>\n");

                  sprintf(str, "%s_2", ua);
                  rsprintf("<span style=\"white-space:nowrap;\">\n");
                  rsprintf
                          ("<input type=radio id=\"%s\" name=\"%s\" value=\"<keep>\" checked onChange=\"mod();\">\n",
                           str, ua);
                  rsprintf("<label for=\"%s\">%s</label>\n", str, loc("keep original values"));
                  rsprintf("</span>\n");
               }

                  /* display checkbox */
               else if (atoi(attrib[index]) == 1)
                  rsprintf
                          ("<td%s class=\"%s\"><input type=checkbox checked name=\"%s\" value=1 onChange=\"mod();\">\n",
                           title, class_value, ua);
               else
                  rsprintf
                          ("<td%s class=\"%s\"><input type=checkbox name=\"%s\" value=1 onChange=\"mod();\">\n",
                           title, class_value, ua);
            } else {

               sprintf(str, "extend_%d", index);
               if (isparam(str)) {

                  rsprintf("<td%s class=\"%s\">\n", title, class_value);
                  rsprintf("<i>");
                  rsprintf(loc("Add new option here"), attr_list[index]);
                  rsprintf("&nbsp;:&nbsp;</i>\n");

                  if (attr_flags[index] & (AF_MULTI | AF_MUSERLIST | AF_MUSEREMAIL))
                     rsprintf
                             ("<input type=\"text\" size=20 maxlength=%d name=\"%s_0\" value=\"%s\" onChange=\"mod();\">\n",
                              input_maxlen, ua, attrib[index]);
                  else
                     rsprintf
                             ("<input type=\"text\" size=20 maxlength=%d name=\"%s\" value=\"%s\" onChange=\"mod();\">\n",
                              input_maxlen, ua, attrib[index]);

                  rsprintf("<input type=\"hidden\" name=\"extend_%d\" value=\"1\">\n", index);
                  rsprintf("</td>\n");

               } else if (attr_flags[index] & AF_MULTI) {

                  /* display multiple check boxes */
                  rsprintf("<td%s class=\"%s\">\n", title, class_value);

                  sprintf(str, "- %s -", loc("keep original values"));
                  if (isparam("nsel") && strieq(attrib[index], str)) {
                     rsprintf("<span style=\"white-space:nowrap;\">\n");
                     sprintf(str, "%s_keep", ua);
                     rsprintf
                             ("<input type=checkbox id=\"%s\" name=\"%s\" value=\"<keep>\" checked onChange=\"mod();\">\n",
                              str, ua);

                     rsprintf("<label for=\"%s\">%s</label>\n", str, loc("keep original values"));
                     rsprintf("</span>\n");
                  }

                  n_moptions = strbreak(attrib[index], attr_moptions, MAX_N_LIST, "|", FALSE);
                  for (i = 0; i < MAX_N_LIST && attr_options[index][i][0]; i++) {

                     /* display check box with optional tooltip */
                     sprintf(str, "Tooltip %s", attr_options[index][i]);
                     tooltip[0] = 0;
                     if (getcfg(lbs->name, str, comment, sizeof(comment)))
                        sprintf(tooltip, " title=\"%s\"", comment);

                     sprintf(str, "Tooltip %s %s", attr_list[index], attr_options[index][i]);
                     tooltip[0] = 0;
                     if (getcfg(lbs->name, str, comment, sizeof(comment)))
                        sprintf(tooltip, " title=\"%s\"", comment);

                     sprintf(str, "%s_%d", ua, i);
                     rsprintf("<span%s style=\"white-space:nowrap;\">\n", tooltip);

                     for (j = 0; j < n_moptions; j++)
                        if (strcmp(attr_moptions[j], attr_options[index][i]) == 0)
                           break;

                     if (j < n_moptions)
                        rsprintf
                                ("<input type=checkbox id=\"%s\" name=\"%s\" value=\"%s\" checked onChange=\"mod();\">\n",
                                 str, str, attr_options[index][i]);
                     else
                        rsprintf
                                ("<input type=checkbox id=\"%s\" name=\"%s\" value=\"%s\" onChange=\"mod();\">\n",
                                 str, str, attr_options[index][i]);

                     rsprintf("<label for=\"%s\">%s</label>\n", str, attr_options[index][i]);
                     rsprintf("</span>\n");

                     if (format_flags[index] & AFF_MULTI_LINE)
                        rsprintf("<br>");
                  }

                  rsprintf("<input type=button value=\"%s\" onClick=\"ToggleAll('%s');\">\n",
                           loc("Toggle all"), ua);

                  if (attr_flags[index] & AF_EXTENDABLE) {
                     sprintf(str, loc("Add %s"), attr_list[index]);
                     rsprintf
                             ("<input type=submit name=\"extend_%d\" value=\"%s\" onClick=\"return mark_submitted();\">\n",
                              index, str);
                  }

                  rsprintf("</td>\n");

               } else if (attr_flags[index] & AF_RADIO) {
                  /* display radio buttons */
                  rsprintf("<td%s class=\"%s\">\n", title, class_value);

                  for (i = 0; i < MAX_N_LIST && attr_options[index][i][0]; i++) {
                     /* display check box with optional tooltip */
                     sprintf(str, "Tooltip %s", attr_options[index][i]);
                     tooltip[0] = 0;
                     if (getcfg(lbs->name, str, comment, sizeof(comment)))
                        sprintf(tooltip, " title=\"%s\"", comment);

                     rsprintf("<span%s style=\"white-space:nowrap;\">\n", tooltip);

                     strencode2(str, attr_options[index][i], sizeof(str));
                     if (strchr(str, '{'))
                        *strchr(str, '{') = 0;
                     strencode2(enc_attr, attrib[index], sizeof(enc_attr));

                     if (strieq(attrib[index], attr_options[index][i]) || strieq(str, enc_attr))
                        rsprintf
                                ("<input type=radio id=\"%s\" name=\"%s\" value=\"%s\" checked onChange=\"mod();\">\n",
                                 str, ua, str);
                     else
                        rsprintf
                                ("<input type=radio id=\"%s\" name=\"%s\" value=\"%s\" onChange=\"mod();\">\n",
                                 str, ua, str);

                     rsprintf("<label for=\"%s\">%s</label>\n", str, str);

                     rsprintf("</span>\n");

                     if (format_flags[index] & AFF_MULTI_LINE)
                        rsprintf("<br>");
                  }

                  if (attr_flags[index] & AF_EXTENDABLE) {
                     sprintf(str, loc("Add %s"), attr_list[index]);
                     rsprintf
                             ("<input type=submit name=\"extend_%d\" value=\"%s\" onClick=\"return mark_submitted();\">\n",
                              index, str);
                  }

                  rsprintf("</td>\n");

               } else if (attr_flags[index] & AF_ICON) {
                  /* display icons */
                  rsprintf("<td%s class=\"%s\">\n", title, class_value);
                  rsprintf("<table cellpadding=\"0\" cellspacing=\"0\"><tr>\n");

                  for (i = 0; i < MAX_N_LIST && attr_options[index][i][0]; i++) {
                     if (strstr(attrib[index], attr_options[index][i]))
                        rsprintf
                                ("<td><input type=radio checked name=\"%s\" value=\"%s\" onChange=\"mod();\">",
                                 ua, attr_options[index][i]);
                     else
                        rsprintf("<td><input type=radio name=\"%s\" value=\"%s\" onChange=\"mod();\">", ua,
                                 attr_options[index][i]);

                     sprintf(str, "Icon comment %s", attr_options[index][i]);
                     getcfg(lbs->name, str, comment, sizeof(comment));

                     if (comment[0])
                        rsprintf("<img src=\"icons/%s\" alt=\"%s\" title=\"%s\">\n", attr_options[index][i],
                                 comment, comment);
                     else
                        rsprintf("<img src=\"icons/%s\" alt=\"%s\" title=\"%s\">\n", attr_options[index][i],
                                 attr_options[index][i], attr_options[index][i]);

                     rsprintf("</td>\n");
                     if ((format_flags[index] & AFF_MULTI_LINE) && attr_options[index][i + 1][0]) {
                        rsprintf("</tr><tr>\n");
                     }
                  }

                  rsprintf("</tr></table></td>\n");

               } else {

                  rsprintf("<td%s class=\"%s\">\n", title, class_value);

                  /* display drop-down box */
                  rsprintf("<select name=\"%s\"", ua);

                  if (is_cond_attr(index))
                     rsprintf(" onChange=\"cond_submit()\">\n");
                  else
                     rsprintf(" onChange=\"mod();\">\n");

                  /* display emtpy option */
                  sprintf(str, "- %s -", loc("keep original values"));
                  if (strcmp(str, attrib[index]) == 0 && isparam("nsel"))
                     rsprintf("<option value=\"<keep>\">%s\n", str);
                  else
                     rsprintf("<option value=\"\">- %s -\n", loc("please select"));

                  for (i = 0; i < MAX_N_LIST && attr_options[index][i][0]; i++) {
                     strencode2(str, attr_options[index][i], sizeof(str));
                     if (strchr(str, '{'))
                        *strchr(str, '{') = 0;
                     strencode2(enc_attr, attrib[index], sizeof(enc_attr));

                     if (strieq(attr_options[index][i], attrib[index]) || strieq(str, enc_attr))
                        rsprintf("<option selected value=\"%s\">%s\n", str, str);
                     else
                        rsprintf("<option value=\"%s\">%s\n", str, str);
                  }

                  rsprintf("</select>\n");

                  if (is_cond_attr(index)) {
                     /* show "update" button only of javascript is not enabled */
                     rsprintf("<noscript>\n");
                     rsprintf("<input type=submit value=\"%s\">\n", loc("Update"));
                     rsprintf("</noscript>\n");
                  }

                  if (attr_flags[index] & AF_EXTENDABLE) {
                     sprintf(str, loc("Add %s"), attr_list[index]);
                     rsprintf
                             ("<input type=submit name=\"extend_%d\" value=\"%s\" onClick=\"return mark_submitted();\">\n",
                              index, str);
                  }

                  rsprintf("</td>\n");
               }
            }
         }
      }

      if (aindex < n_disp_attr - 1 && (format_flags[attr_index[aindex + 1]] & AFF_SAME_LINE) == 0) {
         /* if next attribute not on same line, close row or subtable */
         if (subtable) {
            rsprintf("</table></td></tr>\n");
            subtable = 0;
         } else
            rsprintf("</tr>");
      }

      /* if last attribute, close row or subtable */
      if (aindex == n_disp_attr - 1) {
         if (subtable) {
            rsprintf("</table></td></tr>\n");
            subtable = 0;
         } else
            rsprintf("</tr>");
      }
   }

   if (bpreview && !fixed_text) {
      _current_message_id = message_id;
      rsprintf("<tr><td colspan=2 class=\"messageframe\">\n");

      if (strieq(encoding, "plain")) {
         rsputs("<pre class=\"messagepre\">");
         rsputs2(lbs, FALSE, text);
         rsputs("</pre>");
      } else if (strieq(encoding, "ELCode"))
         rsputs_elcode(lbs, FALSE, text);
      else
         rsputs(text);

      rsprintf("</td></tr>\n");
   }

   if (enc_selected == 0 && show_text && !fixed_text) {
      rsprintf("<tr><td colspan=2 class=\"toolframe\">\n");

      ricon("bold", loc("Bold text CTRL+B"), "elcode(document.form1.Text, 'B','')");
      ricon("italic", loc("Italics text CTRL+I"), "elcode(document.form1.Text, 'I','')");
      ricon("underline", loc("Underlined text CTRL+U"), "elcode(document.form1.Text, 'U','')");

      rsprintf(" ");
      ricon("center", loc("Centered text"), "elcode(document.form1.Text, 'CENTER','')");

      rsprintf(" ");
      ricon("url", loc("Insert hyperlink"), "queryURL(document.form1.Text)");
      ricon("email", loc("Insert email"), "elcode(document.form1.Text, 'EMAIL','')");

      sprintf(str, "window.open('upload.html', '',");
      strlcat(str, "'top=280,left=350,width=500,height=120,dependent=yes,", sizeof(str));
      strlcat(str, "menubar=no,status=no,scrollbars=no,location=no,resizable=yes')", sizeof(str));
      ricon("image", loc("Insert image CTRL+M"), str);

      rsprintf(" ");
      ricon("quote", loc("Insert quote"), "elcode(document.form1.Text, 'QUOTE','')");
      ricon("list", loc("Insert list CTRL+L"), "elcode(document.form1.Text, 'LIST','')");
      ricon("table", loc("Insert table"), "elcode(document.form1.Text, 'TABLE','')");
      ricon("heading", loc("Insert heading CTRL+H"), "queryHeading(document.form1.Text)");
      ricon("line", loc("Insert horizontal line"), "elcode(document.form1.Text, 'LINE','')");
      ricon("anchor", loc("Insert anchor point"), "elcode(document.form1.Text, 'ANCHOR','')");

      rsprintf(" ");
      ricon("code", loc("Insert code CTRL+O"), "elcode(document.form1.Text, 'CODE','')");

      rsprintf(
              " <img align=\"middle\" id=\"smileyIcon\" src=\"icons/elc_smile.png\" alt=\"%s\" title=\"%s\" border=\"0\"",
              loc("Show the smiley bar"), loc("Show the smiley bar"));
      rsprintf(" onclick=\"switch_smileys()\"");
      rsprintf(" onmouseover=\"this.style.cursor='pointer';\" />\n");

      rsprintf(" <select name=\"_font_name\" ");
      rsprintf("onchange=\"elcode(document.form1.Text,'FONT',this.options[this.selectedIndex].value);");
      rsprintf("this.selectedIndex=0;\">\n");
      rsprintf("<option value=\"0\">%s</option>\n", loc("FONT"));

      if (!getcfg(lbs->name, "Fonts", str, sizeof(str)))
         strcpy(str, "Arial, Comic Sans MS, Courier New, Tahoma, Times New Roman, Verdana");

      n = strbreak(str, list, MAX_N_ATTR, ",", FALSE);
      for (i = 0; i < n; i++)
         rsprintf("<option value=\"%s\">%s</option>\n", list[i], list[i]);

      rsprintf("</select>\n");

      rsprintf(" <select name=\"_font_size\" ");
      rsprintf("onchange=\"elcode(document.form1.Text,'SIZE',this.options[this.selectedIndex].value);");
      rsprintf("this.selectedIndex=0;\">\n");
      rsprintf("<option value=\"0\">%s</option>\n", loc("SIZE"));
      rsprintf("<option value=\"1\">1</option>\n");
      rsprintf("<option value=\"2\">2</option>\n");
      rsprintf("<option value=\"3\">3</option>\n");
      rsprintf("<option value=\"4\">4</option>\n");
      rsprintf("<option value=\"5\">5</option>\n");
      rsprintf("<option value=\"6\">6</option>\n");
      rsprintf("</select>\n");

      rsprintf(" <select name=\"_font_color\" ");
      rsprintf("onchange=\"elcode(document.form1.Text,'COLOR',this.options[this.selectedIndex].value);");
      rsprintf("this.selectedIndex=0;\">\n");
      rsprintf("<option value=\"0\">%s</option>\n", loc("COLOR"));
      rsprintf("<option value=\"blue\" style=\"color:blue\">blue</option>\n");
      rsprintf("<option value=\"darkblue\" style=\"color:darkblue\">dark-blue</option>\n");
      rsprintf("<option value=\"orange\" style=\"color:orange\">orange</option>\n");
      rsprintf("<option value=\"red\" style=\"color:red\">red</option>\n");
      rsprintf("<option value=\"darkred\" style=\"color:darkred\">dark red</option>\n");
      rsprintf("<option value=\"green\" style=\"color:green\">green</option>\n");
      rsprintf("<option value=\"darkgreen\" style=\"color:darkgreen\">dark-green</option>\n");
      rsprintf("<option value=\"pink\" style=\"color:deeppink\">pink</option>\n");
      rsprintf("<option value=\"purple\" style=\"color:purple\">purple</option>\n");
      rsprintf("<option value=\"chocolate\" style=\"color:chocolate\">chocolate</option>\n");
      rsprintf("</select>");

      rsprintf(" ");
      ricon("clock", loc("Insert current time/date"), "insertTime(document.form1.Text)");

      rsprintf("</td></tr>\n");
   }

   if (enc_selected == 0) {

      rsprintf("<tr id=\"smileyRow\" style=\"display:none\"><td colspan=2 class=\"toolframe\">\n");
      rsicon("smile", loc("smiling"), ":)");
      rsicon("happy", loc("happy"), ":))");
      rsicon("wink", loc("winking"), ";)");
      rsicon("biggrin", loc("big grin"), ":D");
      rsicon("crying", loc("crying"), ";(");
      rsicon("cool", loc("cool"), "8-)");
      rsicon("frown", loc("frowning"), ":(");
      rsicon("confused", loc("confused"), "?-)");
      rsicon("astonished", loc("astonished"), "8o");
      rsicon("mad", loc("mad"), "X-(");
      rsicon("pleased", loc("pleased"), ":]");
      rsicon("tongue", loc("tongue"), ":P");
      rsicon("yawn", loc("yawn"), ":O");

      rsprintf("</td></tr>\n");
   }

   if (getcfg(lbs->name, "Message comment", comment, sizeof(comment)) && !message_id) {
      rsprintf("<tr><td colspan=2 class=\"attribvalue\">%s</td></tr>\n", comment);
   }

   if (getcfg(lbs->name, "Reply comment", comment, sizeof(comment)) && breply) {
      rsprintf("<tr><td colspan=2 class=\"attribvalue\">%s</td></tr>\n", comment);
   }

   rsprintf(
           "<tr><td colspan=2 width=\"100%%\" class=\"attribvalue\" id=\"TextParent\" style=\"padding:0\">\n");

   /* set textarea width */
   width = 112;

   if (getcfg(lbs->name, "Message width", str, sizeof(str)))
      width = atoi(str);

   /* increased width according to longest line */
   if (message_id && enc_selected == 1) {
      p = text;
      do {
         pend = strchr(p, '\n');
         if (pend == NULL)
            pend = p + strlen(p);

         if (pend - p + 1 > width)
            width = pend - p + 1;

         if (*pend == 0)
            break;

         p = pend;
         while (*p && (*p == '\r' || *p == '\n'))
            p++;
      } while (1);

      /* leave space for '> ' */
      if (!bedit && !bduplicate)
         width += 2;
   }

   /* set textarea height */
   height = 20;
   if (getcfg(lbs->name, "Message height", str, sizeof(str)))
      height = atoi(str);

   if (breply)
      /* hidden text for original message */
      rsprintf("<input type=\"hidden\" name=\"reply_to\" value=\"%d\">\n", message_id);

   if (breedit || bupload)
      /* hidden text for original message */
      if (isparam("reply_to"))
         rsprintf("<input type=\"hidden\" name=\"reply_to\" value=\"%s\">\n", getparam("reply_to"));

   if (bedit && message_id)
      rsprintf("<input type=\"hidden\" id=\"edit_id\" name=\"edit_id\" value=\"%d\">\n", message_id);

   if (isparam("nsel")) {
      rsprintf("<input type=\"hidden\" name=\"nsel\" value=\"%s\">\n", getparam("nsel"));
      for (i = 0; i < atoi(getparam("nsel")); i++) {
         sprintf(str, "s%d", i);
         if (isparam(str)) {
            rsprintf("<input type=\"hidden\" name=\"s%d\" value=\"%s\">\n", i, getparam(str));
         }
      }
   }

   preset_text = getcfg(lbs->name, "Preset text", str, sizeof(str));
   if (preset_text) {

      /* don't use preset text if editing or replying */
      if (bedit || bduplicate || breply)
         preset_text = FALSE;

      /* user preset on reedit only if preset is under condition */
      if (breedit && !bpreview && !bupload && getcfg(lbs->name, "Preset text", str, sizeof(str)) == 2)
         preset_text = TRUE;
   }

   if (show_text) {

      if (fixed_text) {

         rsprintf("<input type=hidden name=\"text\" value=\"<keep>\">\n");
         rsprintf("<input type=hidden name=\"encoding\" value=\"%s\">\n", encoding);

         _current_message_id = message_id;
         rsprintf("<tr><td colspan=2 class=\"messageframe\">\n");

         if (strieq(text, "<keep>") && message_id) {
            size = TEXT_SIZE;
            el_retrieve(lbs, message_id, NULL, NULL, NULL, 0, text, &size, NULL, NULL, NULL, NULL, NULL, NULL);
         }

         if (strieq(encoding, "plain")) {
            rsputs("<pre class=\"messagepre\">");
            rsputs2(lbs, FALSE, text);
            rsputs("</pre>");
         } else if (strieq(encoding, "ELCode"))
            rsputs_elcode(lbs, FALSE, text);
         else
            rsputs(text);

         rsprintf("</td></tr>\n");

         rsprintf("<tr><td colspan=2 width=\"100%%\" class=\"attribvalue\">\n");

      } else {

         if (enc_selected == 1)
            /* use hard wrapping only for plain text */
            rsprintf("<textarea rows=%d cols=%d wrap=hard name=\"Text\">\n", height, width);
         else
            rsprintf
                    ("<textarea rows=%d cols=%d name=\"Text\" style=\"width:100%%;\">\n", height, width);

         if (isparam("nsel")) {
            rsprintf("- %s -\n", loc("keep original text"));
         } else if (bedit) {
            if (!preset_text) {

               j = build_subst_list(lbs, slist, svalue, attrib, TRUE);
               sprintf(mid, "%d", message_id);
               add_subst_list(slist, svalue, "message id", mid, &j);
               add_subst_time(lbs, slist, svalue, "entry time", date, &j, 0);

               if (!bupload)
                  if (getcfg(lbs->name, "Prepend on edit", str, sizeof(str))) {
                     strsubst_list(str, sizeof(str), slist, svalue, j);
                     while (strstr(str, "\\n"))
                        memcpy(strstr(str, "\\n"), "\r\n", 2);
                     rsputs3(str);
                  }

               /* use rsputs3 which just converts "<", ">", "&", "\"" to &gt; etc. */
               /* otherwise some HTML statments would break the page syntax        */
               rsputs3(text);

               if (!bupload)
                  if (getcfg(lbs->name, "Append on edit", str, sizeof(str))) {
                     strsubst_list(str, sizeof(str), slist, svalue, j);
                     while (strstr(str, "\\n"))
                        memcpy(strstr(str, "\\n"), "\r\n", 2);
                     rsputs3(str);
                  }
            }
         } else if (bduplicate) {

            rsputs3(text);

         } else if (breply) {
            if (!getcfg(lbs->name, "Quote on reply", str, sizeof(str)) || atoi(str) > 0) {
               if (getcfg(lbs->name, "Prepend on reply", str, sizeof(str))) {
                  j = build_subst_list(lbs, slist, svalue, attrib, TRUE);
                  sprintf(mid, "%d", message_id);
                  add_subst_list(slist, svalue, "message id", mid, &j);
                  add_subst_time(lbs, slist, svalue, "entry time", date, &j, 0);

                  strsubst_list(str, sizeof(str), slist, svalue, j);
                  while (strstr(str, "\\n"))
                     memcpy(strstr(str, "\\n"), "\r\n", 2);
                  rsputs3(str);
               }

               p = text;

               if (text[0]) {
                  if (!getcfg(lbs->name, "Reply string", reply_string, sizeof(reply_string)))
                     strcpy(reply_string, "> ");

                  if (enc_selected == 0) {

                     /* check for author */
                     if (orig_author[0])
                        rsprintf("[quote=\"%s\"]", orig_author);
                     else
                        rsprintf("[quote]");
                     rsputs3(text);
                     rsprintf("[/quote]\r\n");

                  } else if (enc_selected == 2) {

                     rsprintf("<p>\n");
                     rsprintf
                             ("<table width=\"98%%\" align=\"center\" cellspacing=\"1\" style=\"border:1px solid #486090;\">\n");
                     rsprintf("<tbody>\n");
                     rsprintf("<tr>\n");
                     rsprintf
                             ("<td cellpadding=\"3px\" style=\"background-color:#486090; font-weidht:bold; color:white;\">");

                     /* check for author */
                     if (orig_author[0]) {
                        rsprintf(loc("%s wrote"), orig_author);
                     } else {
                        rsprintf(loc("Quote"));
                     }
                     rsprintf(":</td></tr>\n");
                     rsprintf("<tr>\n");
                     rsprintf("<td cellpadding=\"10px\" style=\"background-color:#FFFFB0;\">");
                     rsputs3(text);
                     rsprintf("</td>\n");
                     rsprintf("</tr>\n");
                     rsprintf("</tbody>\n");
                     rsprintf("</table>\n");
                     rsprintf("</p><p>&nbsp;</p>\n");

                  } else {
                     do {
                        if (strchr(p, '\n')) {
                           *strchr(p, '\n') = 0;

                           if (encoding[0] == 'H') {
                              rsputs3(reply_string);
                              rsprintf("%s<br>\n", p);
                           } else {
                              rsputs(reply_string);
                              rsputs3(p);
                              rsprintf("\n");
                           }

                           p += strlen(p) + 1;
                           if (*p == '\n')
                              p++;
                        } else {
                           if (encoding[0] == 'H') {
                              rsputs3(reply_string);
                              rsprintf("%s<p>\n", p);
                           } else {
                              rsputs(reply_string);
                              rsputs3(p);
                              rsprintf("\n\n");
                           }

                           break;
                        }

                     } while (TRUE);
                  }
               }

               if (getcfg(lbs->name, "Append on reply", str, sizeof(str))) {

                  j = build_subst_list(lbs, slist, svalue, attrib, TRUE);
                  sprintf(mid, "%d", message_id);
                  add_subst_list(slist, svalue, "message id", mid, &j);
                  add_subst_time(lbs, slist, svalue, "entry time", date, &j, 0);
                  strsubst_list(str, sizeof(str), slist, svalue, j);
                  while (strstr(str, "\\n"))
                     memcpy(strstr(str, "\\n"), "\r\n", 2);
                  rsputs3(str);
               }
            }
         }

         if (preset_text && !isparam("nsel")) {
            getcfg(lbs->name, "Preset text", str, sizeof(str));

            /* check if file starts with an absolute directory */
            if (str[0] == DIR_SEPARATOR || str[1] == ':')
               strcpy(file_name, str);
            else {
               strlcpy(file_name, logbook_dir, sizeof(file_name));
               strlcat(file_name, str, sizeof(file_name));
            }

            /* check if file exists */
            fh = open(file_name, O_RDONLY | O_BINARY);
            if (fh > 0) {
               length = lseek(fh, 0, SEEK_END);
               lseek(fh, 0, SEEK_SET);
               buffer = xmalloc(length + 1);
               read(fh, buffer, length);
               buffer[length] = 0;
               close(fh);
               rsputs3(buffer);
               xfree(buffer);
            } else {
               j = build_subst_list(lbs, slist, svalue, attrib, TRUE);
               strsubst_list(str, sizeof(str), slist, svalue, j);
               while (strstr(str, "\\n"))
                  memcpy(strstr(str, "\\n"), "\r\n", 2);

               if (strchr(str, ' ')) {
                  /* name contains blanks -> must be text */
                  rsputs3(str);
               } else {
                  /* name is probably a file -> show error */
                  rsputs("File <i>");
                  rsputs3(str);
                  rsputs("</i> cannot be found");
               }
            }
         }

         rsprintf("</textarea><br>\n");

         /* Encoding radio buttons */

         if (allowed_encoding < 1 || allowed_encoding > 7) {
            rsprintf
                    ("<h1>Invalid \"Allowed encoding\" in configuration file, value must be between 1 and 7</h1>\n");
            rsprintf("</table><!-- show_standard_title -->\n");
            show_bottom_text(lbs);
            rsprintf("</form></body></html>\r\n");
            return;
         }

         if (allowed_encoding == 1)
            rsprintf("<input type=\"hidden\" name=\"encoding\" value=\"plain\">\n");
         else if (allowed_encoding == 2)
            rsprintf("<input type=\"hidden\" name=\"encoding\" value=\"ELCode\">\n");
         else if (allowed_encoding == 4)
            rsprintf("<input type=\"hidden\" name=\"encoding\" value=\"HTML\">\n");
         else {
            if (allowed_encoding == 4)
               rsprintf("<input type=hidden name=\"encoding\" value=\"HTML\">\n");
            else if (allowed_encoding == 2)
               rsprintf("<input type=hidden name=\"encoding\" value=\"ELCode\">\n");
            else if (allowed_encoding == 1)
               rsprintf("<input type=hidden name=\"encoding\" value=\"plain\">\n");
            else {
               rsprintf("<b>%s</b>: ", loc("Encoding"));

               if (allowed_encoding & 4) {
                  if (enc_selected == 2)
                     rsprintf
                             ("<input type=radio id=\"HTML\" name=\"encoding\" value=\"HTML\" checked=\"checked\">");
                  else
                     rsprintf
                             ("<input type=radio id=\"HTML\" name=\"encoding\" value=\"HTML\" onclick=\"cond_submit()\">");
                  rsprintf("<label for=\"HTML\">HTML&nbsp;&nbsp;</label>\n");
               }

               if (allowed_encoding & 2) {
                  if (enc_selected == 0)
                     rsprintf("<input type=radio id=\"ELCode\" name=\"encoding\" value=\"ELCode\" checked>");
                  else
                     rsprintf
                             ("<input type=radio id=\"ELCode\" name=\"encoding\" value=\"ELCode\" onclick=\"cond_submit()\">");
                  rsprintf
                          ("<label for=\"ELCode\"><a target=\"_blank\" href=\"?cmd=HelpELCode\">ELCode</a>&nbsp;&nbsp;</label>\n");
               }

               if (allowed_encoding & 1) {
                  if (enc_selected == 1)
                     rsprintf("<input type=radio id=\"plain\" name=\"encoding\" value=\"plain\" checked>");
                  else
                     rsprintf
                             ("<input type=radio id=\"plain\" name=\"encoding\" value=\"plain\" onclick=\"cond_submit()\">");
                  rsprintf("<label for=\"plain\">plain&nbsp;&nbsp;</label>\n");
               }
            }
         }

         rsprintf("<br>\n");
      }
   }

   /* Suppress email check box */
   if (message_id && bedit)
      getcfg(lbs->name, "Suppress Email on edit", str, sizeof(str));
   else
      getcfg(lbs->name, "Suppress default", str, sizeof(str));

   if (atoi(str) == 0) {
      rsprintf("<input type=\"checkbox\" name=\"suppress\" id=\"suppress\" value=\"1\">");
      rsprintf("<label for=\"suppress\">%s</label>\n", loc("Suppress Email notification"));
   } else if (atoi(str) == 1) {
      rsprintf("<input type=\"checkbox\" checked name=\"suppress\" id=\"suppress\" value=\"1\">");
      rsprintf("<label for=\"suppress\">%s</label>\n", loc("Suppress Email notification"));
   } else if (atoi(str) == 2) {
      rsprintf("<input type=\"hidden\" name=\"suppress\" id=\"suppress\" value=\"2\">");
   } else if (atoi(str) == 3) {
      rsprintf("<input type=\"hidden\" name=\"suppress\" id=\"suppress\" value=\"3\">");
   }

   /* Suppress execute shell check box */
   if (!bedit && getcfg(lbs->name, "Execute new", str, sizeof(str))) {
      if (getcfg(lbs->name, "Suppress execute default", str, sizeof(str))) {
         if (atoi(str) == 0) {
            rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
            rsprintf("<input type=\"checkbox\" name=\"shell_suppress\" id=\"shell_suppress\" value=1>");
            rsprintf("<label for=\"shell_suppress\">%s</label>\n", loc("Suppress shell execution"));
         } else if (atoi(str) == 1) {
            rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
            rsprintf
                    ("<input type=\"checkbox\" checked name=\"shell_suppress\" id=\"shell_suppress\" value=1>");
            rsprintf("<label for=\"shell_suppress\">%s</label>\n", loc("Suppress shell execution"));
         }
      } else {
         rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
         rsprintf("<input type=\"checkbox\" name=\"shell_suppress\" id=\"shell_suppress\" value=1>");
         rsprintf("<label for=\"shell_suppress\">%s</label>\n", loc("Suppress shell execution"));
      }
   }

   if (bedit && getcfg(lbs->name, "Execute edit", str, sizeof(str))) {
      if (getcfg(lbs->name, "Suppress execute default", str, sizeof(str))) {
         if (atoi(str) == 0) {
            rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
            rsprintf("<input type=\"checkbox\" name=\"shell_suppress\" id=\"shell_suppress\" value=1>");
            rsprintf("<label for=\"shell_suppress\">%s</label>\n", loc("Suppress shell execution"));
         } else if (atoi(str) == 1) {
            rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
            rsprintf
                    ("<input type=\"checkbox\" checked name=\"shell_suppress\" id=\"shell_suppress\" value=1>");
            rsprintf("<label for=\"shell_suppress\">%s</label>\n", loc("Suppress shell execution"));
         }
      } else {
         rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
         rsprintf("<input type=\"checkbox\" name=\"shell_suppress\" id=\"shell_suppress\" value=1>");
         rsprintf("<label for=\"shell_suppress\">%s</label>\n", loc("Suppress shell execution"));
      }
   }

   /* Resubmit check box */
   if (bedit && message_id) {
      if (getcfg(lbs->name, "Resubmit default", str, sizeof(str))) {
         if (atoi(str) == 0) {
            rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
            rsprintf("<input type=\"checkbox\" name=\"resubmit\" id=\"resubmit\" value=1>");
            rsprintf("<label for=\"resubmit\">%s</label>\n", loc("Resubmit as new entry"));
         } else if (atoi(str) == 1) {
            rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
            rsprintf("<input type=\"checkbox\" checked name=\"resubmit\" id=\"resubmit\" value=1>");
            rsprintf("<label for=\"resubmit\">%s</label>\n", loc("Resubmit as new entry"));
         }
      } else {
         rsprintf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
         rsprintf("<input type=\"checkbox\" name=\"resubmit\" id=\"resubmit\" value=1>");
         rsprintf("<label for=\"resubmit\">%s</label>\n", loc("Resubmit as new entry"));
      }
   }

   rsprintf("</tr>\n");

   for (i = 0; i < MAX_ATTACHMENTS; i++)
      if (!att[i][0]) {
         /* put first free attachment for show_uploader_finished() */
         rsprintf("<tr><td><input type=hidden name=\"next_attachment\" value=\"%d\"></td></tr>\n", i + 1);
         break;
      }

   index = 0;
   if (!getcfg(lbs->name, "Enable attachments", str, sizeof(str)) || atoi(str) > 0) {
      if (bedit || bduplicate || isparam("fa")) {
         /* show existing attachments */
         for (index = 0; index < MAX_ATTACHMENTS; index++)
            if (att[index][0]) {
               rsprintf("<tr class=\"attachment\"><td nowrap class=\"attribname\">%s %d:</td>\n",
                        loc("Attachment"), index + 1);
               sprintf(str, "attachment%d", index);
               rsprintf("<td class=\"attribvalue\">\n");
               thumb_ref[0] = 0;

               if (strlen(att[index]) < 14 || att[index][6] != '_' || att[index][13] != '_') {
                  strencode2(str, att[index], sizeof(str));
                  rsprintf("<b>Error: Invalid attachment \"%s\"</b><br>", str);
               } else {

                  strlcpy(file_name, lbs->data_dir, sizeof(file_name));
                  generate_subdir_name(att[index], subdir, sizeof(subdir));
                  strlcat(file_name, subdir, sizeof(file_name));
                  strlcat(file_name, att[index], sizeof(file_name));

                  display_inline = is_image(file_name) || is_ascii(file_name);
                  if (chkext(file_name, ".ps") || chkext(file_name, ".pdf") || chkext(file_name, ".eps")
                      || chkext(file_name, ".svg"))
                     display_inline = 0;
                  if ((chkext(file_name, ".htm") || chkext(file_name, ".html")) && is_full_html(file_name))
                     display_inline = 0;

                  thumb_status = create_thumbnail(lbs, file_name);
                  if (thumb_status)
                     display_inline = 1;

                  if (getcfg(lbs->name, "Preview attachments", str, sizeof(str)) && atoi(str) == 0)
                     display_inline = 0;

                  if (thumb_status && display_inline) {
                     get_thumb_name(file_name, thumb_name, sizeof(thumb_name), 0);
                     if (strrchr(thumb_name, DIR_SEPARATOR))
                        strlcpy(str, strrchr(thumb_name, DIR_SEPARATOR) + 1, sizeof(str));
                     else
                        strlcpy(str, thumb_name, sizeof(str));
                     strlcpy(thumb_name, str, sizeof(thumb_name));
                     if (thumb_status == 2)
                        strsubst(thumb_name, sizeof(thumb_name), "-0.png", "");

                     rsprintf("<table><tr><td class=\"toolframe\">\n");
                     sprintf(str, "im('att'+'%d','%s','%s','smaller');", index, thumb_name, att[index]);
                     ricon("smaller", loc("Make smaller"), str);
                     sprintf(str, "im('att'+'%d','%s','%s','original');", index, thumb_name, att[index]);
                     ricon("original", loc("Original size"), str);
                     sprintf(str, "im('att'+'%d','%s','%s','larger');", index, thumb_name, att[index]);
                     ricon("larger", loc("Make larger"), str);
                     rsprintf("&nbsp;\n");
                     sprintf(str, "im('att'+'%d','%s','%s','rotleft');", index, thumb_name, att[index]);
                     ricon("rotleft", loc("Rotate left"), str);
                     sprintf(str, "im('att'+'%d','%s','%s','rotright');", index, thumb_name, att[index]);
                     ricon("rotright", loc("Rotate right"), str);
                     rsprintf("&nbsp;\n");
                     sprintf(str, "deleteAtt('%d','%s')", index,
                             loc("Are you sure to delete the attachment?"));
                     ricon("delatt", loc("Delete attachment"), str);
                     rsprintf("&nbsp;&nbsp;\n");

                     /* ImageMagick available, so get image size */
                     strencode2(str, att[index], sizeof(str));
                     rsprintf("<b>%s</b>&nbsp;\n", str + 14);
                     if (chkext(file_name, ".pdf") || chkext(file_name, ".ps"))
                        sprintf(cmd, "%s -format '%%wx%%h' '%s[0]'", _identify_cmd, file_name);
                     else
                        sprintf(cmd, "%s -format '%%wx%%h' '%s'", _identify_cmd, file_name);
#ifdef OS_WINNT
                                                                                                                                             for (i = 0; i < (int) strlen(cmd); i++)
                        if (cmd[i] == '\'')
                           cmd[i] = '\"';
#endif
                     my_shell(cmd, str, sizeof(str));
                     if (atoi(str) > 0)
                        rsprintf("<span class=\"bytes\">%s: %s</span>\n", loc("Original size"), str);

                     rsprintf("</td></tr>\n");
                     rsprintf("<tr><td align=center>");
                  } else {
                     rsprintf("%s\n", att[index] + 14);
                     rsprintf("&nbsp;&nbsp;<input type=\"submit\" name=\"delatt%d\" value=\"%s\" ", index,
                              loc("Delete"));
                     rsprintf("onClick=\"return mark_submitted();\">");
                     rsprintf("<br>\n");
                  }

                  if (display_inline) {

                     if (is_image(att[index]) || thumb_status) {
                        if (thumb_status == 1) {
                           get_thumb_name(file_name, thumb_name, sizeof(thumb_name), 0);
                           strlcpy(str, att[index], sizeof(str));
                           str[13] = 0;
                           if (strrchr(thumb_name, DIR_SEPARATOR))
                              strlcpy(file_enc, strrchr(thumb_name, DIR_SEPARATOR) + 1 + 14,
                                      sizeof(file_enc));
                           else
                              strlcpy(file_enc, thumb_name + 14, sizeof(file_enc));
                           url_encode(file_enc,
                                      sizeof(file_enc));      /* for file names with special characters like "+" */
                           sprintf(ref, "%s/%s?thumb=1", str, file_enc);

                           rsprintf("<img src=\"%s\" alt=\"%s\" title=\"%s\" name=\"att%d\">\n", ref,
                                    att[index] + 14, att[index] + 14, index);
                           strlcpy(thumb_ref, ref, sizeof(thumb_ref));
                        } else if (thumb_status == 2) {
                           for (i = 0;; i++) {
                              get_thumb_name(file_name, thumb_name, sizeof(thumb_name), i);
                              if (thumb_name[0]) {
                                 strlcpy(str, att[index], sizeof(str));
                                 str[13] = 0;
                                 if (strrchr(thumb_name, DIR_SEPARATOR))
                                    strlcpy(file_enc, strrchr(thumb_name, DIR_SEPARATOR) + 1 + 14,
                                            sizeof(file_enc));
                                 else
                                    strlcpy(file_enc, thumb_name + 14, sizeof(file_enc));
                                 url_encode(file_enc,
                                            sizeof(file_enc));        /* for file names with special characters like "+" */
                                 sprintf(ref, "%s/%s?thumb=1", str, file_enc);

                                 rsprintf("<img src=\"%s\" alt=\"%s\" title=\"%s\" name=\"att%d_%d\">\n",
                                          ref, att[index] + 14, att[index] + 14, index, i);
                                 strlcpy(thumb_ref, ref, sizeof(thumb_ref));
                              } else
                                 break;
                           }

                        } else {
                           strlcpy(str, att[index], sizeof(str));
                           str[13] = 0;
                           strcpy(file_enc, att[index] + 14);
                           url_encode(file_enc,
                                      sizeof(file_enc));      /* for file names with special characters like "+" */
                           sprintf(ref, "%s/%s", str, file_enc);

                           rsprintf("<img src=\"%s\" alt=\"%s\" title=\"%s\" name=\"att%d\">\n", ref,
                                    att[index] + 14, att[index] + 14, index);
                           strlcpy(thumb_ref, ref, sizeof(thumb_ref));
                        }
                     } else {
                        if (is_ascii(file_name)) {
                           if (!chkext(att[index], ".HTML"))
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
                                    if (!chkext(att[index], ".HTML"))
                                       rsputs2(lbs, FALSE, str);
                                    else
                                       rsputs(str);
                                 }
                                 n_lines++;
                              }
                              fclose(f);
                           }

                           if (!chkext(att[index], ".HTML"))
                              rsprintf("</pre>");
                           rsprintf("\n");
                           if (max_n_lines == 0)
                              rsprintf("<i><b>%d lines</b></i>\n", n_lines);
                           else if (n_lines > max_n_lines)
                              rsprintf("<i><b>... %d more lines ...</b></i>\n", n_lines - max_n_lines);
                        }
                     }
                  }

                  if (thumb_status && display_inline)
                     rsprintf("</td></tr></table>\n");
               }

               strencode2(str, att[index], sizeof(str));
               if (thumb_ref[0])
                  rsprintf("<input type=hidden name=\"attachment%d\" alt=\"%s\" value=\"%s\">\n", index,
                           thumb_ref, str);
               else
                  rsprintf("<input type=hidden name=\"attachment%d\" value=\"%s\">\n", index, str);

               rsprintf("</td></tr>\n");
            } else
               break;
      }

      /* optional attachment comment */
      if (getcfg(lbs->name, "Attachment comment", comment, sizeof(comment))) {
         rsprintf("<tr><td colspan=2 class=\"attribvalue\">\n");
         rsputs(comment);
         rsputs("</td></tr>\n");
      }

      /* field for add attachment */
      if (att[MAX_ATTACHMENTS - 1][0]) {
         rsprintf("<tr><td colspan=2 class=\"attribname\">%s</td>\n",
                  loc("Maximum number of attachments reached"));
         rsprintf("</td></tr>\n");
      } else {
         rsprintf("<tr id=\"attachment_upload\"><td nowrap class=\"attribname\">%s %d:</td>\n",
                  loc("Attachment"), index + 1);
         rsprintf(
                 "<td class=\"attribvalue\"><input type=\"file\" size=\"60\" maxlength=\"200\" name=\"attfile\">\n");
         rsprintf(
                 "&nbsp;&nbsp;<input type=\"submit\" name=\"cmd\" value=\"%s\" onClick=\"return chkupload();\">\n",
                 loc("Upload"));
         rsprintf("</td></tr>\n");

         // print the holder for dropping attachments
         rsprintf("<tr>\n");
         rsprintf("<td style=\"background: white;\" colspan=2>\n");
         rsprintf("<div id=\"holder\" class=\"holder\">%s</div>", loc("Drop attachments here..."));
         rsprintf("</td></tr>");
      }
   }

   rsprintf("</table><!-- listframe -->\n");

   /*---- menu buttons again ----*/

   rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");
   rsprintf("<input type=\"submit\" name=\"cmd\" value=\"%s\" onClick=\"return chkform(this);\">\n",
            loc("Submit"));

   if (!getcfg(lbs->name, "Save drafts", str, sizeof(str)) || atoi(str) == 1)
      rsprintf("<input type=\"submit\" name=\"cmd\" value=\"%s\" onClick=\"return save_draft();\">\n",
               loc("Save"));

   if (!getcfg(lbs->name, "Show text", str, sizeof(str)) || atoi(str) == 1)
      rsprintf("<input type=\"submit\" name=\"cmd\" value=\"%s\" onClick=\"return mark_submitted();\">\n",
               loc("Preview"));

   if (!getcfg(lbs->name, "Save drafts", str, sizeof(str)) || atoi(str) == 1)
      rsprintf("<input type=\"submit\" name=\"cmd\" value=\"%s\" onClick=\"return check_delete();\">\n",
               loc("Delete"));
   else
      rsprintf("<input type=\"submit\" name=\"cmd\" value=\"%s\" onClick=\"return mark_submitted();\">\n",
               loc("Back"));

   rsprintf(
           "&nbsp;&nbsp;<span id=\"saved2\" style=\"font-size:10px;font-style:italic;display:none\">%s 00:00:00</span>",
           loc("Draft saved at"));

   rsprintf("</span></td></tr>\n\n");

   rsprintf("</table><!-- show_standard_title -->\n");
   show_bottom_text(lbs);
   rsprintf("</form></body></html>\r\n");

   /* rescan unconditional attributes */
   if (_condition[0])
      scan_attributes(lbs->name);

   xfree(text);
}