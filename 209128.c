char *xstrdup(const char *string) {
   char *s;

   s = (char *) xmalloc(strlen(string) + 1);
   strcpy(s, string);
   return s;
}