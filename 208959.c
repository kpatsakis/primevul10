void compose_base_url(LOGBOOK *lbs, char *base_url, int size, BOOL email_notify)
/* return URL for elogd with optional logbook subdirectory */
{
   if (email_notify)
      if (getcfg(lbs->name, "Use Email URL", base_url, size))
         return;

   if (!getcfg("global", "URL", base_url, size)) {

      if (referer[0]) {
         /* get URL from referer */
         strlcpy(base_url, referer, size);
         if (strchr(base_url, '?'))
            *strchr(base_url, '?') = 0;
         if (strrchr(base_url, '/'))
            *(strrchr(base_url, '/') + 1) = 0;
      } else {
         if (_ssl_flag)
            strcpy(base_url, "https://");
         else
            strcpy(base_url, "http://");

         if (elog_tcp_port == 80)
            sprintf(base_url + strlen(base_url), "%s/", host_name);
         else
            sprintf(base_url + strlen(base_url), "%s:%d/", host_name, elog_tcp_port);

         if (lbs) {
            strlcat(base_url, lbs->name_enc, size);
            strlcat(base_url, "/", size);
         }
      }
   } else {
      if (base_url[strlen(base_url) - 1] != '/')
         strlcat(base_url, "/", size);
      if (lbs) {
         strlcat(base_url, lbs->name_enc, size);
         strlcat(base_url, "/", size);
      }
   }
}