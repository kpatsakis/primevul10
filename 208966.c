static BOOL chkext(const char *str, const char *ext) {
   int extl, strl;
   char c1, c2;

   if (ext == NULL || str == NULL)
      return FALSE;

   extl = strlen(ext);
   strl = strlen(str);
   if (extl >= strl)
      return FALSE;
   str = str + strl - extl;
   while (*str) {
      c1 = *str++;
      c2 = *ext++;
      if (my_toupper(c1) != my_toupper(c2))
         return FALSE;
   }
   return TRUE;
}