void extract_path(char *str) {
   char *p, str2[256];

   p = NULL;

   if (strstr(str, "http://"))
      p = str + 7;
   if (strstr(str, "https://"))
      p = str + 8;

   if (p) {
      while (*p && *p != '/')
         p++;
      if (*p == '/')
         p++;

      strcpy(str2, p);
      strcpy(str, str2);
      if (str[strlen(str) - 1] == '/')
         str[strlen(str) - 1] = 0;
   }
}