BOOL is_author(LOGBOOK *lbs, char attrib[MAX_N_ATTR][NAME_LENGTH], char *owner) {
   char str[NAME_LENGTH], preset[NAME_LENGTH], full_name[NAME_LENGTH];
   int i;

   /* check if current user is admin */
   if (is_admin_user(lbs, getparam("unm")))
      return TRUE;

   /* search attribute which contains short_name of author */
   for (i = 0; i < lbs->n_attr; i++) {
      sprintf(str, "Preset %s", attr_list[i]);
      if (getcfg(lbs->name, str, preset, sizeof(preset))) {
         if (strstr(preset, "$short_name")) {
            if (!isparam("unm") || strstr(attrib[i], getparam("unm")) == NULL) {
               strcpy(owner, attrib[i]);
               return FALSE;
            } else
               break;
         }
      }
   }

   if (i == lbs->n_attr) {
      /* if not found, search attribute which contains full_name of author */
      if (isparam("unm")) {
         get_full_name(lbs, getparam("unm"), full_name);
         for (i = 0; i < lbs->n_attr; i++) {
            sprintf(str, "Preset %s", attr_list[i]);
            if (getcfg(lbs->name, str, preset, sizeof(preset))) {
               if (strstr(preset, "$long_name")) {
                  if (strstr(attrib[i], full_name) == NULL) {
                     strcpy(owner, attrib[i]);
                     return FALSE;
                  } else
                     break;
               }
            }
         }
      }
   }

   return TRUE;
}