void extract_host(char *str) {
   char *p, *ph, str2[256];

   p = NULL;

   if (strstr(str, "http://"))
      p = str + 7;
   else if (strstr(str, "https://"))
      p = str + 8;

   if (p) {
      ph = p;
      while (*p && *p != '/' && *p != ':')
         p++;
      *p = 0;

      strcpy(str2, ph);
      strcpy(str, str2);
   }
}