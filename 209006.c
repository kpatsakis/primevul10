void add_logbook_to_group(LOGBOOK *lbs, char *new_name) {
   int i, j, n, flag;
   char str[1000], grpname[256], grpmembers[1000];
   char grplist[MAX_N_LIST][NAME_LENGTH];

   /* enumerate groups */
   for (i = 0;; i++) {
      if (!enumcfg("global", grpname, sizeof(grpname), grpmembers, sizeof(grpmembers), i))
         break;

      flag = 0;
      strlcpy(str, grpname, sizeof(str));
      str[9] = 0;
      if (strieq(str, "top group"))
         flag = 2;
      str[5] = 0;
      if (strieq(str, "group"))
         flag = 1;

      if (flag) {

         n = strbreak(grpmembers, grplist, MAX_N_LIST, ",", FALSE);
         for (j = 0; j < n; j++) {
            if (strieq(lbs->name, grplist[j])) {
               /* rename or remove logbook */
               change_config_line(lbs, grpname, "", new_name);
               break;
            }
         }
      }
   }
}