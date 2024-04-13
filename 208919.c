void strip_html(char *s) {
   char *p;

   while ((p = strchr(s, '<')) != NULL) {
      if (strchr(p, '>'))
         memmove(p, strchr(p, '>') + 1, strlen(strchr(p, '>') + 1) + 1);
      else
         *p = 0;
   }
}