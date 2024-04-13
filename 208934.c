BOOL is_command_allowed(LOGBOOK *lbs, char *command, int message_id) {
   char str[1000], menu_str[1000], other_str[1000];
   char menu_item[MAX_N_LIST][NAME_LENGTH];
   int i, n;

   if (command[0] == 0)
      return TRUE;

   /* check for guest access */
   if (!getcfg(lbs->name, "Guest Menu commands", menu_str, sizeof(menu_str)) || logged_in(lbs))
      getcfg(lbs->name, "Menu commands", menu_str, sizeof(menu_str));

   /* default menu commands */
   if (menu_str[0] == 0) {
      strcpy(menu_str, "List, New, Edit, Delete, Reply, Duplicate, Synchronize, Find, ");

      if (getcfg(lbs->name, "Password file", str, sizeof(str))) {

         if (is_admin_user(lbs, getparam("unm"))) {

            strcat(menu_str, "Admin, ");
            strcat(menu_str, "Change config file, ");
            strcat(menu_str, "Delete this logbook, ");
            strcat(menu_str, "Rename this logbook, ");
            strcat(menu_str, "Create new logbook, ");
            strcat(menu_str, "GetPwdFile, ");

            if (is_admin_user(NULL, getparam("unm"))) {

               if (lbs->top_group[0]) {
                  sprintf(str, "Change [global %s]", lbs->top_group);
                  strcat(menu_str, str);
                  strcat(menu_str, ", ");
               }

               if (!lbs->top_group[0] || (is_admin_user(NULL, getparam("unm")))) {

                  strcat(menu_str, "Change [global]");
                  strcat(menu_str, ", ");
               }
            }
         }
         strcat(menu_str, "Config, Logout, ");
      } else {
         strcat(menu_str, "Config, ");
         strcat(menu_str, "Change [global], ");
         strcat(menu_str, "Delete this logbook, ");
         strcat(menu_str, "Rename this logbook, ");
         strcat(menu_str, "Create new logbook, ");
      }

      strcat(menu_str, "Help, HelpELCode, ");

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
         strcat(menu_str, ", ");
      }

      strcat(menu_str, "HelpELCode, Synchronize, ");

      if (is_admin_user(lbs, getparam("unm"))) {

         strcat(menu_str, "Change config file, ");
         strcat(menu_str, "Delete this logbook, ");
         strcat(menu_str, "Rename this logbook, ");
         strcat(menu_str, "Create new logbook, ");
         strcat(menu_str, "GetPwdFile, ");

         if (is_admin_user(NULL, getparam("unm"))) {

            if (lbs->top_group[0]) {
               sprintf(str, "Change [global %s]", lbs->top_group);
               strcat(menu_str, str);
               strcat(menu_str, ", ");
            }

            if (!lbs->top_group[0] || (is_admin_user(NULL, getparam("unm")))) {

               strcat(menu_str, "Change [global]");
               strcat(menu_str, ", ");
            }
         }
      }
   }

   /* check list menu commands */
   str[0] = 0;
   if (!getcfg(lbs->name, "Guest List Menu commands", str, sizeof(str)) || logged_in(lbs))
      getcfg(lbs->name, "list menu commands", str, sizeof(str));

   if (!str[0]) {
      if (!getcfg(lbs->name, "Guest Find Menu commands", str, sizeof(str)) || logged_in(lbs))
         getcfg(lbs->name, "Find Menu commands", str, sizeof(str));
   }

   if (str[0])
      strlcat(menu_str, str, sizeof(menu_str));
   else {
      strlcat(menu_str, "New, Find, Select, Last x, Help, HelpELCode, ", sizeof(menu_str));

      if (getcfg(lbs->name, "Password file", str, sizeof(str)))
         strlcat(menu_str, "Admin, Config, Logout, ", sizeof(menu_str));
      else
         strlcat(menu_str, "Config, ", sizeof(menu_str));
   }

   strlcpy(other_str, "Preview, Back, Search, Download, Import, CSV Import, XML Import, ", sizeof(other_str));
   strlcat(other_str, "Cancel, First, Last, Previous, Next, Requested, Forgot, ", sizeof(other_str));

   /* only allow Submit & Co if "New" is allowed */
   if (stristr(menu_str, "New,") || stristr(menu_str, "Edit"))
      strlcat(other_str, "Update, Upload, Submit, Save, ", sizeof(other_str));

   /* add save for new user registration */
   if (isparam("new_user_name"))
      strlcat(other_str, "Save, ", sizeof(other_str));

   /* admin commands */
   if (is_admin_user(lbs, getparam("unm"))) {
      strlcat(other_str, "Remove user, New user, Activate, ", sizeof(other_str));
   } else if (getcfg(lbs->name, "Self register", str, sizeof(str)) && atoi(str) > 0) {
      strlcat(other_str, "Remove user, New user, ", sizeof(other_str));
   }

   /* allow always edit of draft messages */
   if (is_draft(lbs, message_id))
      strlcat(other_str, "Edit, ", sizeof(other_str));

   /* allow change password if "config" possible */
   if (strieq(command, loc("Change password")) && stristr(menu_str, "Config")) {
      return TRUE;
   }
      /* exclude non-localized submit for elog */
   else if (command[0] && strieq(command, "Submit")
            && (stristr(menu_str, "New,") || stristr(menu_str, "Edit"))) {
      return TRUE;
   }
      /* exclude other non-localized commands */
   else if (command[0] && strieq(command, "GetMD5")) {
      return TRUE;
   } else if (command[0] && strieq(command, "IM")) {
      return TRUE;
   }
      /* check if command is present in the menu list */
   else if (command[0]) {
      n = strbreak(menu_str, menu_item, MAX_N_LIST, ",", FALSE);
      for (i = 0; i < n; i++)
         if (strieq(command, menu_item[i]) || strieq(command, loc(menu_item[i])))
            break;

      if (i == n) {
         n = strbreak(other_str, menu_item, MAX_N_LIST, ",", FALSE);
         for (i = 0; i < n; i++)
            if (strieq(command, menu_item[i]) || strieq(command, loc(menu_item[i])))
               break;

         if (i == n)
            return FALSE;
      }
   }

   return TRUE;
}