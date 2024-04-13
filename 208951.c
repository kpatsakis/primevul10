void el_delete_attachment(LOGBOOK *lbs, char *file_name) {
   int i;
   char str[MAX_PATH_LENGTH], subdir[MAX_PATH_LENGTH];

   strlcpy(str, lbs->data_dir, sizeof(str));
   generate_subdir_name(file_name, subdir, sizeof(subdir));
   strlcat(str, subdir, sizeof(str));
   strlcat(str, file_name, sizeof(str));
   remove(str);
   strlcat(str, ".png", sizeof(str));
   remove(str);
   for (i = 0;; i++) {
      strlcpy(str, lbs->data_dir, sizeof(str));
      strlcat(str, subdir, sizeof(str));
      strlcat(str, file_name, sizeof(str));
      sprintf(str + strlen(str), "-%d.png", i);
      if (file_exist(str)) {
         remove(str);
         continue;
      }

      strlcpy(str, lbs->data_dir, sizeof(str));
      strlcat(str, subdir, sizeof(str));
      strlcat(str, file_name, sizeof(str));
      if (strrchr(str, '.'))
         *strrchr(str, '.') = 0;
      sprintf(str + strlen(str), "-%d.png", i);
      if (file_exist(str)) {
         remove(str);
         continue;
      }
      break;
   }
}