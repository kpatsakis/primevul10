int save_md5(LOGBOOK *lbs, char *server, MD5_INDEX *md5_index, int n) {
   char str[256], url[256], file_name[256];
   int i, j;
   FILE *f;

   combine_url(lbs, server, "", url, sizeof(url), NULL);
   url_decode(url);
   if (strstr(url, "http://"))
      strlcpy(str, url + 7, sizeof(str));
   else if (strstr(url, "https://"))
      strlcpy(str, url + 8, sizeof(str));
   else
      strlcpy(str, url, sizeof(str));

   for (i = 0; i < (int) strlen(str); i++)
      if (strchr(":/\\ ", str[i]))
         str[i] = '_';

   while (str[strlen(str) - 1] == '_')
      str[strlen(str) - 1] = 0;

   strlcpy(file_name, logbook_dir, sizeof(file_name));
   strlcat(file_name, str, sizeof(file_name));
   strlcat(file_name, ".md5", sizeof(file_name));

   f = fopen(file_name, "wt");
   if (f == NULL)
      return -1;

   for (i = 0; i < n; i++) {
      fprintf(f, "ID%d: ", md5_index[i].message_id);
      for (j = 0; j < 16; j++)
         fprintf(f, "%02X", md5_index[i].md5_digest[j]);
      fprintf(f, "\n");
   }

   fclose(f);
   return 1;
}