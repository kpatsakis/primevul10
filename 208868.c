char *find_param(char *buf, char *group, char *param) {
   char *str, *p, *pstr, *pstart;

   /* search group */
   str = xmalloc(10000);
   p = buf;
   do {
      if (*p == '[') {
         p++;
         pstr = str;
         while (*p && *p != ']' && *p != '\n')
            *pstr++ = *p++;
         *pstr = 0;
         if (strieq(str, group)) {
            /* search parameter */
            p = strchr(p, '\n');
            if (p)
               p++;
            while (p && *p && *p != '[') {
               pstr = str;
               pstart = p;
               while (*p && *p != '=' && *p != '\n')
                  *pstr++ = *p++;
               *pstr-- = 0;
               while (pstr > str && (*pstr == ' ' || *pstr == '=' || *pstr == '\t'))
                  *pstr-- = 0;

               if (match_param(str, param, FALSE)) {
                  xfree(str);
                  return pstart;
               }

               if (p)
                  p = strchr(p, '\n');
               if (p)
                  p++;
            }
         }
      }
      if (p)
         p = strchr(p, '\n');
      if (p)
         p++;
   } while (p);

   xfree(str);

   /* now search if in [global] section */
   if (!strieq(group, "global"))
      return find_param(buf, "global", param);

   return NULL;
}