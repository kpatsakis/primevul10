int max_attachment_size(LOGBOOK *lbs, int message_id) {
   char att_file[MAX_ATTACHMENTS][MAX_PATH_LENGTH];
   int index, max_size, fh;
   char str[256], subdir[256], file_name[256];

   el_retrieve(lbs, message_id, NULL, NULL, NULL, 0, NULL, 0, NULL, NULL, att_file, NULL, NULL, NULL);

   max_size = 0;
   for (index = 0; index < MAX_ATTACHMENTS; index++) {

      if (att_file[index][0] == 0)
         continue;

      strlcpy(file_name, lbs->data_dir, sizeof(file_name));
      generate_subdir_name(att_file[index], subdir, sizeof(subdir));
      strlcat(file_name, subdir, sizeof(file_name));
      strlcat(file_name, att_file[index], sizeof(file_name));
      if (is_image(file_name)) {
         get_thumb_name(file_name, str, sizeof(str), 0);
         if (str[0])
            strlcpy(file_name, str, sizeof(file_name));
      }

      fh = open(file_name, O_RDONLY | O_BINARY);
      if (fh > 0) {
         off_t size = lseek(fh, 0L, SEEK_END);
         max_size = size > max_size ? size : max_size;
      }
   }

   return max_size;
}