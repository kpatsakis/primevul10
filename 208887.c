int is_script(char *s) {
   char *str;
   int i;

   str = xstrdup(s);

   for (i = 0; i < (int) strlen(s); i++)
      str[i] = tolower(s[i]);
   str[i] = 0;

   for (i = 0; script_tags[i] != NULL; i++) {
      if (strstr(str, script_tags[i])) {
         xfree(str);
         return TRUE;
      }
   }

   xfree(str);
   return FALSE;
}