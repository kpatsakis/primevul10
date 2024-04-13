int load_md5(LOGBOOK *lbs, char *server, MD5_INDEX **md5_index) {
   char str[256], url[256], file_name[256], *p;
   int i, j, x;
   FILE *f;

   *md5_index = NULL;

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

   f = fopen(file_name, "rt");
   if (f == NULL)
      return 0;

   for (i = 0; !feof(f); i++) {
      str[0] = 0;
      fgets(str, sizeof(str), f);
      if (!str[0])
         break;

      if (i == 0)
         *md5_index = (MD5_INDEX *) xcalloc(sizeof(MD5_INDEX), 1);
      else
         *md5_index = (MD5_INDEX *) xrealloc(*md5_index, sizeof(MD5_INDEX) * (i + 1));

      p = str + 2;

      (*md5_index)[i].message_id = atoi(p);
      while (*p && *p != ' ')
         p++;
      while (*p && *p == ' ')
         p++;

      for (j = 0; j < 16; j++) {
         sscanf(p + j * 2, "%02X", &x);
         (*md5_index)[i].md5_digest[j] = (unsigned char) x;
      }
   }

   fclose(f);
   return i;
}