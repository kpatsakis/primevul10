void strsubst(char *string, int size, char *pattern, char *subst)
/* subsitute "pattern" with "subst" in "string" */
{
   char *tail, *p;
   int s;

   p = string;
   for (p = stristr(p, pattern); p != NULL; p = stristr(p, pattern)) {

      if (strlen(pattern) == strlen(subst)) {
         memcpy(p, subst, strlen(subst));
      } else if (strlen(pattern) > strlen(subst)) {
         memcpy(p, subst, strlen(subst));
         memmove(p + strlen(subst), p + strlen(pattern), strlen(p + strlen(pattern)) + 1);
      } else {
         tail = (char *) xmalloc(strlen(p) - strlen(pattern) + 1);
         strcpy(tail, p + strlen(pattern));
         s = size - (p - string);
         strlcpy(p, subst, s);
         strlcat(p, tail, s);
         xfree(tail);
      }

      p += strlen(subst);
   }
}