int show_download_page(LOGBOOK *lbs, char *path) {
   char file_name[256], error_str[256];
   int index, message_id, fh, i, size, delta;
   char message[TEXT_SIZE + 1000], *p, *buffer;

   if (stricmp(path, "gbl") == 0) {

      /* return complete config file */
      load_config_section(NULL, &buffer, error_str);
      if (error_str[0]) {
         rsprintf("<h2>%s</h2>", error_str);
         return EL_FILE_ERROR;
      }

      size = strlen(buffer);
      strlcpy(message, buffer, sizeof(message));
      xfree(buffer);

   } else {

      message_id = atoi(path);

      if (message_id == 0) {

         /* return config */
         load_config_section(lbs->name, &buffer, error_str);
         if (error_str[0]) {
            rsprintf("<h2>%s</h2>", error_str);
            return EL_FILE_ERROR;
         }

         size = strlen(buffer);
         strlcpy(message, buffer, sizeof(message));
         xfree(buffer);

      } else {

         /* return entry */

         if (message_id == -1)
            index = *lbs->n_el_index - 1;  // last entry
         else {
            for (index = 0; index < *lbs->n_el_index; index++)
               if (lbs->el_index[index].message_id == message_id)
                  break;
         }

         if (index == *lbs->n_el_index)
            return EL_NO_MSG;

         snprintf(file_name, sizeof(file_name), "%s%s%s", lbs->data_dir, lbs->el_index[index].subdir,
                  lbs->el_index[index].file_name);
         fh = open(file_name, O_RDWR | O_BINARY, 0644);
         if (fh < 0)
            return EL_FILE_ERROR;

         lseek(fh, lbs->el_index[index].offset, SEEK_SET);
         i = my_read(fh, message, sizeof(message) - 1);
         if (i <= 0) {
            close(fh);
            return EL_FILE_ERROR;
         }

         message[i] = 0;
         close(fh);

         /* decode message size */
         p = strstr(message + 8, "$@MID@$:");
         if (p == NULL)
            size = strlen(message);
         else
            size = p - message;

         message[size] = 0;
      }
   }

   show_plain_header(size, "export.txt");

   /* increase return buffer size if file too big */
   if (size + 1 >= return_buffer_size - (int) strlen(return_buffer)) {
      delta = size - (return_buffer_size - strlen(return_buffer)) + 1000;

      return_buffer = (char *) xrealloc(return_buffer, return_buffer_size + delta);
      memset(return_buffer + return_buffer_size, 0, delta);
      return_buffer_size += delta;
   }

   return_length = strlen(return_buffer) + size;
   strlcat(return_buffer, message, return_buffer_size);

   return EL_SUCCESS;
}