void retrieve_email_from(LOGBOOK *lbs, char *ret, char *ret_name, char attrib[MAX_N_ATTR][NAME_LENGTH]) {
   char email_from[256], email_from_name[256], str[256], *p, login_name[256],
           slist[MAX_N_ATTR + 10][NAME_LENGTH], svalue[MAX_N_ATTR + 10][NAME_LENGTH],
           full_name[256], user_email[256];
   int i;

   if (getcfg(lbs->name, "Use Email from", str, sizeof(str))) {
      if (str[0] != '<') {
         strlcpy(email_from, "<", sizeof(email_from));
         strlcat(email_from, str, sizeof(email_from));
         strlcat(email_from, ">", sizeof(email_from));
      } else
         strlcpy(email_from, str, sizeof(email_from));
      strlcpy(email_from_name, str, sizeof(email_from));
   } else if (isparam("unm")) {
      get_user_line(lbs, getparam("unm"), NULL, full_name, user_email, NULL, NULL, NULL);
      strlcpy(email_from_name, full_name, sizeof(email_from_name));
      strlcat(email_from_name, " <", sizeof(email_from_name));
      strlcat(email_from_name, user_email, sizeof(email_from_name));
      strlcat(email_from_name, ">", sizeof(email_from_name));
      strlcpy(email_from, "<", sizeof(email_from));
      strlcat(email_from, user_email, sizeof(email_from));
      strlcat(email_from, ">", sizeof(email_from));
   } else if (getcfg(lbs->name, "Default Email from", str, sizeof(str))) {
      if (str[0] != '<') {
         strlcpy(email_from, "<", sizeof(email_from));
         strlcat(email_from, str, sizeof(email_from));
         strlcat(email_from, ">", sizeof(email_from));
      } else
         strlcpy(email_from, str, sizeof(email_from));
      strlcpy(email_from_name, str, sizeof(email_from));
   } else {
      sprintf(email_from_name, "ELog <ELog@%s>", host_name);
      sprintf(email_from, "<ELog@%s>", host_name);
   }

   if (attrib) {
      i = build_subst_list(lbs, slist, svalue, attrib, TRUE);
      strsubst_list(email_from_name, sizeof(email_from_name), slist, svalue, i);
      strsubst_list(email_from, sizeof(email_from), slist, svalue, i);

      /* remove possible 'mailto:' */
      if ((p = strstr(email_from_name, "mailto:")) != NULL)
         memmove(p, p + 7, strlen(p + 7) + 1);
      if ((p = strstr(email_from, "mailto:")) != NULL)
         memmove(p, p + 7, strlen(p + 7) + 1);
   }

   /* if nothing available, figure out email from an administrator */
   if (strchr(email_from, '@') == NULL) {
      for (i = 0;; i++) {
         if (!enum_user_line(lbs, i, login_name, sizeof(login_name)))
            break;
         get_user_line(lbs, login_name, NULL, NULL, email_from, NULL, NULL, NULL);
         sprintf(email_from_name, "%s <%s>", login_name, email_from);
         if (is_admin_user(lbs, login_name) && strchr(email_from, '@'))
            break;
      }
   }

   if (ret)
      strcpy(ret, email_from);
   if (ret_name)
      strcpy(ret_name, email_from_name);
}