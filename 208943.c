int scan_attributes(char *logbook)
/* scan configuration file for attributes and fill attr_list, attr_options
 and attr_flags arrays */
{
   char list[10000], str[NAME_LENGTH], str2[NAME_LENGTH], type[NAME_LENGTH],
           tmp_list[MAX_N_ATTR][NAME_LENGTH];
   int i, j, n, m, n_options;

   if (getcfg(logbook, "Attributes", list, sizeof(list))) {
      /* reset attribute flags */
      memset(attr_flags, 0, sizeof(attr_flags));

      /* get attribute list */
      memset(attr_list, 0, sizeof(attr_list));
      n = strbreak(list, attr_list, MAX_N_ATTR, ",", FALSE);

      /* check for forbidden attributes */
      for (i = 0; i < n; i++) {
         if (strieq(attr_list[i], "id") ||
             strieq(attr_list[i], "text") ||
             strieq(attr_list[i], "date") ||
             strieq(attr_list[i], "encoding") ||
             strieq(attr_list[i], "reply to") ||
             strieq(attr_list[i], "locked by") ||
             strieq(attr_list[i], "in reply to") || strieq(attr_list[i], "attachment")) {
            snprintf(str, sizeof(str), loc("Attribute \"%s\" is not allowed in config file"), attr_list[i]);
            show_error(str);
            return -1;
         }
      }

      /* get options lists for attributes */
      memset(attr_options, 0, sizeof(attr_options));
      for (i = 0; i < n; i++) {
         n_options = 0;

         snprintf(str, sizeof(str), "Options %s", attr_list[i]);
         if (getcfg(logbook, str, list, sizeof(list)))
            n_options = strbreak(list, attr_options[i], MAX_N_LIST, ",", FALSE);

         snprintf(str, sizeof(str), "MOptions %s", attr_list[i]);
         if (getcfg(logbook, str, list, sizeof(list))) {
            n_options = strbreak(list, attr_options[i], MAX_N_LIST, ",", FALSE);
            attr_flags[i] |= AF_MULTI;
         }

         snprintf(str, sizeof(str), "ROptions %s", attr_list[i]);
         if (getcfg(logbook, str, list, sizeof(list))) {
            n_options = strbreak(list, attr_options[i], MAX_N_LIST, ",", FALSE);
            attr_flags[i] |= AF_RADIO;
         }

         snprintf(str, sizeof(str), "IOptions %s", attr_list[i]);
         if (getcfg(logbook, str, list, sizeof(list))) {
            n_options = strbreak(list, attr_options[i], MAX_N_LIST, ",", FALSE);
            attr_flags[i] |= AF_ICON;
         }

         snprintf(str2, sizeof(str2), "Sort Attribute Options %s", attr_list[i]);
         if (n_options && getcfg(logbook, str2, str, sizeof(str)) && atoi(str) == 1) {
            qsort(attr_options[i], n_options, NAME_LENGTH, ascii_compare2);
         }
      }

      /* check if attribute required */
      getcfg(logbook, "Required Attributes", list, sizeof(list));
      m = strbreak(list, tmp_list, MAX_N_ATTR, ",", FALSE);
      for (i = 0; i < m; i++) {
         for (j = 0; j < n; j++)
            if (strieq(attr_list[j], tmp_list[i]))
               attr_flags[j] |= AF_REQUIRED;
      }

      /* check if locked attribute */
      getcfg(logbook, "Locked Attributes", list, sizeof(list));
      m = strbreak(list, tmp_list, MAX_N_ATTR, ",", FALSE);
      for (i = 0; i < m; i++) {
         for (j = 0; j < n; j++)
            if (strieq(attr_list[j], tmp_list[i]))
               attr_flags[j] |= AF_LOCKED;
      }

      /* check if fixed attribute for Edit */
      getcfg(logbook, "Fixed Attributes Edit", list, sizeof(list));
      m = strbreak(list, tmp_list, MAX_N_ATTR, ",", FALSE);
      for (i = 0; i < m; i++) {
         for (j = 0; j < n; j++)
            if (strieq(attr_list[j], tmp_list[i]))
               attr_flags[j] |= AF_FIXED_EDIT;
      }

      /* check if fixed attribute for Reply */
      getcfg(logbook, "Fixed Attributes Reply", list, sizeof(list));
      m = strbreak(list, tmp_list, MAX_N_ATTR, ",", FALSE);
      for (i = 0; i < m; i++) {
         for (j = 0; j < n; j++)
            if (strieq(attr_list[j], tmp_list[i]))
               attr_flags[j] |= AF_FIXED_REPLY;
      }

      /* check for extendable options */
      getcfg(logbook, "Extendable Options", list, sizeof(list));
      m = strbreak(list, tmp_list, MAX_N_ATTR, ",", FALSE);
      for (i = 0; i < m; i++) {
         for (j = 0; j < n; j++)
            if (strieq(attr_list[j], tmp_list[i]))
               attr_flags[j] |= AF_EXTENDABLE;
      }

      for (i = 0; i < n; i++) {
         snprintf(str, sizeof(str), "Type %s", attr_list[i]);
         if (getcfg(logbook, str, type, sizeof(type))) {
            if (strieq(type, "date"))
               attr_flags[i] |= AF_DATE;
            if (strieq(type, "datetime"))
               attr_flags[i] |= AF_DATETIME;
            if (strieq(type, "time"))
               attr_flags[i] |= AF_TIME;
            if (strieq(type, "numeric"))
               attr_flags[i] |= AF_NUMERIC;
            if (strieq(type, "userlist"))
               attr_flags[i] |= AF_USERLIST;
            if (strieq(type, "muserlist"))
               attr_flags[i] |= AF_MUSERLIST;
            if (strieq(type, "useremail"))
               attr_flags[i] |= AF_USEREMAIL;
            if (strieq(type, "museremail"))
               attr_flags[i] |= AF_MUSEREMAIL;
         }
      }

   } else {
      memcpy(attr_list, attr_list_default, sizeof(attr_list_default));
      memcpy(attr_options, attr_options_default, sizeof(attr_options_default));
      memcpy(attr_flags, attr_flags_default, sizeof(attr_flags_default));
      n = 4;
   }

   return n;
}