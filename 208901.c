void retrieve_domain(char *ret, int size) {
   char smtp_host[80];

   strlcpy(ret, "tmp.org", size);
   if (getcfg("global", "SMTP host", smtp_host, sizeof(smtp_host))) {
      if (strchr(smtp_host, '.'))
         strlcpy(ret, strchr(smtp_host, '.') + 1, size);
   }
}