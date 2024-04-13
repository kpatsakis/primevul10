void split_url(const char *url, char *host, int *port, char *subdir, char *param) {
   const char *p;
   char str[256];

   if (host)
      host[0] = 0;
   if (port)
      *port = 80;
   if (subdir)
      subdir[0] = 0;
   if (param)
      param[0] = 0;

   p = url;
   if (strncmp(url, "http://", 7) == 0)
      p += 7;
   if (strncmp(url, "https://", 8) == 0)
      p += 8;

   strncpy(str, p, sizeof(str));
   if (strchr(str, '/')) {
      if (subdir)
         strncpy(subdir, strchr(str, '/'), 256);
      *strchr(str, '/') = 0;
   }

   if (strchr(str, '?')) {
      if (subdir)
         strncpy(subdir, strchr(str, '?'), 256);
      *strchr(str, '?') = 0;
   }

   if (strchr(str, ':')) {
      if (port)
         *port = atoi(strchr(str, ':') + 1);
      *strchr(str, ':') = 0;
   }

   if (host)
      strcpy(host, str);

   if (subdir) {
      if (strchr(subdir, '?')) {
         strncpy(param, strchr(subdir, '?'), 256);
         *strchr(subdir, '?') = 0;
      }

      if (subdir[0] == 0)
         strcpy(subdir, "/");
   }
}