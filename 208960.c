int get_thumb_name(const char *file_name, char *thumb_name, int size, int index) {
   char str[MAX_PATH_LENGTH];

   thumb_name[0] = 0;

   /* append .png for all files as thumbnail name, except for PDF files (convert bug!) */
   memset(str, 0, sizeof(str));
   if (chkext(file_name, ".pdf") || chkext(file_name, ".ps")) {
      strlcpy(str, file_name, sizeof(str));
      if (strrchr(str, '.'))
         *strrchr(str, '.') = 0;
      snprintf(str + strlen(str), sizeof(str) - strlen(str) - 1, "-%d.png", index);
      if (file_exist(str)) {
         strlcpy(thumb_name, str, size);
         return 2;
      }

      if (index > 0)
         return 0;

      strlcpy(str, file_name, sizeof(str));
      if (strrchr(str, '.'))
         *strrchr(str, '.') = 0;
      strlcat(str, ".png", sizeof(str));
      if (file_exist(str)) {
         strlcpy(thumb_name, str, size);
         return 1;
      }
   } else {
      strlcpy(str, file_name, sizeof(str));
      sprintf(str + strlen(str), "-%d.png", index);
      if (file_exist(str)) {
         strlcpy(thumb_name, str, size);
         return 2;
      }

      if (index > 0)
         return 0;

      strlcpy(str, file_name, sizeof(str));
      strlcat(str, ".png", sizeof(str));
      if (file_exist(str)) {
         strlcpy(thumb_name, str, size);
         return 1;
      }
   }

   return 0;
}