void set_redir(LOGBOOK *lbs, char *redir) {
   char str[NAME_LENGTH];

   str[0] = 0;

   /* prepare relative path */
   if (redir[0])
      strlcpy(str, redir, sizeof(str));
   else {
      if (lbs)
         snprintf(str, sizeof(str), "../%s/", lbs->name_enc);
      else if (getcfg_topgroup())
         sprintf(str, ".");
   }

   set_location(lbs, str);
}