char *loc(char *orig) {
   int n;
   char language[256];
   static char result[256];

   if (!_locbuffer)
      return orig;

   /* search string and return translation */
   for (n = 0; _porig[n]; n++)
      if (strcmp(orig, _porig[n]) == 0) {
         if (*_ptrans[n])
            return _ptrans[n];
         return orig;
      }

   /* special case: "Change %s" */
   if (strstr(orig, "Change ") && strcmp(orig, "Change %s") != 0) {
      sprintf(result, loc("Change %s"), orig + 7);
      return result;
   }

   /* special case: some intrinsic commands */
   if (strstr(orig, "GetPwdFile")) {
      strcpy(result, orig);
      return result;
   }

   getcfg("global", "Language", language, sizeof(language));
   eprintf("Language error: string \"%s\" not found for language \"%s\"\n", orig, language);

   return orig;
}