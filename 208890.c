void combine_url(LOGBOOK *lbs, char *url, char *param, char *result, int size, int *ssl) {
   if (ssl)
      *ssl = 0;
   if (strstr(url, "http://"))
      strlcpy(result, url + 7, size);
   else if (strstr(url, "https://")) {
      if (ssl)
         *ssl = 1;
      strlcpy(result, url + 8, size);
   } else
      strlcpy(result, url, size);

   url_encode(result, size);

   if (result[strlen(result) - 1] != '/')
      strlcat(result, "/", size);

   if (lbs != NULL) {
      if (!strstr(result, lbs->name_enc)) {
         strlcat(result, lbs->name_enc, size);
         strlcat(result, "/", size);
      }
   }

   if (param)
      strlcat(result, param, size);
}