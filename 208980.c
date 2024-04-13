int is_html(char *s) {
   char *str, *p;
   int i;

   str = xstrdup(s);

   for (i = 0; i < (int) strlen(s); i++)
      str[i] = toupper(s[i]);
   str[i] = 0;

   for (i = 0; html_tags[i][0]; i++) {
      p = strstr(str, html_tags[i]);
      if (p && strchr(p, '>') && (p == str || (p > str && *(p - 1) != '\\'))) {
         xfree(str);
         return TRUE;
      }
   }

   if (strstr(str, "&#") && strchr(strstr(str, "&#"), ';')) {
      xfree(str);
      return TRUE;
   }

   xfree(str);
   return FALSE;
}