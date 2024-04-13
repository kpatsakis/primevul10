void load_config_section(char *section, char **buffer, char *error) {
   int fh, length;
   char *p;

   error[0] = 0;
   *buffer = NULL;
   fh = open(config_file, O_RDONLY | O_BINARY);
   if (fh < 0) {
      sprintf(error, "Cannot read configuration file \"%s\": %s", config_file, strerror(errno));
      return;
   }
   length = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   *buffer = xmalloc(length + 1);
   read(fh, *buffer, length);
   (*buffer)[length] = 0;
   close(fh);

   if (section == NULL)
      return;

   if ((p = (char *) find_section(*buffer, section)) != NULL) {
      if (strchr(p, ']')) {
         p = strchr(p, ']') + 1;
         while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
            p++;
      }
      memmove(*buffer, p, length);
      if ((p = (char *) find_next_section(*buffer + 1)) != NULL) {
         *p = 0;

         /* strip trailing newlines */
         if (p) {
            p--;

            while (p > *buffer && (*p == '\n' || *p == '\r' || *p == ' ' || *p == '\t'))
               *p-- = 0;
         }
      } else {
         p = *buffer + strlen(*buffer) - 1;

         while (p > *buffer && (*p == '\n' || *p == '\r' || *p == ' ' || *p == '\t'))
            *p-- = 0;
      }
   }
}