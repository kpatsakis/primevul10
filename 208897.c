int download_config() {
   char error_str[256];
   int size, delta;
   char message[TEXT_SIZE + 1000], *buffer;

   /* return complete config file */
   load_config_section(NULL, &buffer, error_str);
   if (error_str[0]) {
      rsprintf("Error loading configuration file: %s", error_str);
      return EL_FILE_ERROR;
   }

   size = strlen(buffer);
   strlcpy(message, buffer, sizeof(message));
   xfree(buffer);

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