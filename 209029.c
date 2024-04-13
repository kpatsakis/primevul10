BOOL get_author(LOGBOOK *lbs, char attrib[MAX_N_ATTR][NAME_LENGTH], char *author) {
   char str[NAME_LENGTH], preset[NAME_LENGTH];
   int i;

   /* search attribute which contains full_name of author */
   for (i = 0; i < lbs->n_attr; i++) {
      sprintf(str, "Preset %s", attr_list[i]);
      if (getcfg(lbs->name, str, preset, sizeof(preset))) {
         if (stristr(preset, "$long_name")) {
            strcpy(author, attrib[i]);
            return TRUE;
         }
      }
   }

   /* if not found, search attribute which contains short_name of author */
   for (i = 0; i < lbs->n_attr; i++) {
      sprintf(str, "Preset %s", attr_list[i]);
      if (getcfg(lbs->name, str, preset, sizeof(preset))) {
         if (stristr(preset, "$short_name")) {
            strcpy(author, attrib[i]);
            return TRUE;
         }
      }
   }

   return FALSE;
}