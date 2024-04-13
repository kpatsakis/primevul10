BOOL strieq(const char *str1, const char *str2) {
   char c1, c2;

   if (str1 == NULL && str2 == NULL)
      return TRUE;
   if (str1 == NULL || str2 == NULL)
      return FALSE;
   if (strlen(str1) != strlen(str2))
      return FALSE;

   while (*str1) {
      c1 = *str1++;
      c2 = *str2++;
      if (my_toupper(c1) != my_toupper(c2))
         return FALSE;
   }

   if (*str2)
      return FALSE;

   return TRUE;
}