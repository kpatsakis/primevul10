BOOL strnieq(const char *str1, const char *str2, int n) {
   char c1, c2;
   int i;

   if (str1 == NULL && str2 == NULL && n == 0)
      return TRUE;
   if (str1 == NULL || str2 == NULL)
      return FALSE;
   if ((int) strlen(str1) < n || (int) strlen(str2) < n)
      return FALSE;

   for (i = 0; i < n && *str1; i++) {
      c1 = *str1++;
      c2 = *str2++;
      if (my_toupper(c1) != my_toupper(c2))
         return FALSE;
   }

   if (i < n)
      return FALSE;

   return TRUE;
}