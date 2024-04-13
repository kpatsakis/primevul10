BOOL match_param(char *str, char *param, int conditional_only) {
   int ncl, npl, nand, i, j, k;
   char *p, pcond[256], clist[10][NAME_LENGTH], plist[10][NAME_LENGTH], alist[10][NAME_LENGTH];

   if (conditional_only && str[0] != '{')
      return FALSE;

   if (!_condition[0] || str[0] != '{')
      return (stricmp(str, param) == 0);

   p = str;
   if (strchr(p, '}'))
      p = strchr(p, '}') + 1;
   while (*p == ' ')
      p++;

   strlcpy(pcond, str, sizeof(pcond));
   if (strchr(pcond, '}'))
      *strchr(pcond, '}') = 0;
   if (strchr(pcond, '{'))
      *strchr(pcond, '{') = ' ';

   npl = strbreak(pcond, plist, 10, ",", FALSE);
   ncl = strbreak(_condition, clist, 10, ",", FALSE);

   for (i = 0; i < ncl; i++)
      for (j = 0; j < npl; j++)
         if (stricmp(clist[i], plist[j]) == 0) {
            /* condition matches */
            return stricmp(p, param) == 0;
         }

   /* check and'ed conditions */
   for (i = 0; i < npl; i++)
      if (strchr(plist[i], '&')) {
         nand = strbreak(plist[i], alist, 10, "&", FALSE);
         for (j = 0; j < nand; j++) {
            for (k = 0; k < ncl; k++)
               if (stricmp(clist[k], alist[j]) == 0)
                  break;

            if (k == ncl)
               return FALSE;
         }

         if (j == nand)
            return stricmp(p, param) == 0;
      }

   return 0;
}