void convert_elog_link(LOGBOOK *lbs, char *link, char *link_text, char *result, int absolute_link,
                       int message_id) {
   char str[256], base_url[256];
   int i;

   strlcpy(str, link, sizeof(str));
   if (strchr(str, '/'))
      *strchr(str, '/') = 0;

   for (i = 0; i < (int) strlen(str); i++)
      if (!isdigit(str[i]))
         break;

   if (i < (int) strlen(str)) {
      /* if link contains reference to other logbook, put logbook explicitly */
      if (absolute_link)
         compose_base_url(NULL, base_url, sizeof(base_url), FALSE);
      else
         strcpy(base_url, "../");
      sprintf(result, "<a href=\"%s%s\">elog:%s</a>", base_url, link, link_text);
   } else if (link[0] == '/') {
      if (absolute_link)
         compose_base_url(NULL, base_url, sizeof(base_url), FALSE);
      else
         base_url[0] = 0;
      sprintf(result, "<a href=\"%s%s/%d%s\">elog:%s</a>", base_url, lbs->name_enc, message_id, link,
              link_text);
   } else {
      if (absolute_link)
         compose_base_url(lbs, base_url, sizeof(base_url), FALSE);
      else
         base_url[0] = 0;
      sprintf(result, "<a href=\"%s%s\">elog:%s</a>", base_url, link, link_text);
   }
}