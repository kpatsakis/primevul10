int el_submit_attachment(LOGBOOK *lbs, const char *afilename, const char *buffer, int buffer_size,
                         char *full_name) {
   char file_name[MAX_PATH_LENGTH], ext_file_name[
           MAX_PATH_LENGTH + 100], str[MAX_PATH_LENGTH], *p, subdir[MAX_PATH_LENGTH];
   int fh;
   time_t now;
   struct tm tms;

   /* strip directory, add date and time to filename */
   strlcpy(str, afilename, sizeof(str));
   p = str;
   while (strchr(p, ':'))
      p = strchr(p, ':') + 1;
   while (strchr(p, '\\'))
      p = strchr(p, '\\') + 1;  /* NT */
   while (strchr(p, '/'))
      p = strchr(p, '/') + 1;   /* Unix */
   strlcpy(file_name, p, sizeof(file_name));

   /* assemble ELog filename */
   if (file_name[0]) {

      if (file_name[6] == '_' && file_name[13] == '_' && isdigit(file_name[0]) && isdigit(file_name[1]))
         strlcpy(ext_file_name, file_name, sizeof(ext_file_name));
      else {
         time(&now);
         memcpy(&tms, localtime(&now), sizeof(struct tm));

         sprintf(ext_file_name, "%02d%02d%02d_%02d%02d%02d_%s", tms.tm_year % 100, tms.tm_mon + 1,
                 tms.tm_mday, tms.tm_hour, tms.tm_min, tms.tm_sec, file_name);
      }

      if (full_name)
         strlcpy(full_name, ext_file_name, MAX_PATH_LENGTH);

      strlcpy(str, lbs->data_dir, sizeof(str));
      generate_subdir_name(ext_file_name, subdir, sizeof(subdir));
      strlcat(str, subdir, sizeof(str));
      if (strlen(str) > 0 && str[strlen(str) - 1] == DIR_SEPARATOR)
         str[strlen(str) - 1] = 0;

#ifdef OS_WINNT
      mkdir(str);
#else
      mkdir(str, 0755);
#endif

      strlcat(str, DIR_SEPARATOR_STR, sizeof(str));
      strlcat(str, ext_file_name, sizeof(str));

      /* save attachment */
      fh = open(str, O_CREAT | O_RDWR | O_BINARY, 0644);
      if (fh < 0) {
         strencode2(file_name, str, sizeof(file_name));
         sprintf(str, "Cannot write attachment file \"%s\"", file_name);
         show_error(str);
         return -1;
      } else {
         write(fh, buffer, buffer_size);
         close(fh);
      }
   }

   return 0;
}