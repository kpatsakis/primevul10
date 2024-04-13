int save_config(char *buffer, char *error) {
   int fh, i;
   char *buf;

   error[0] = 0;

   fh = open(config_file, O_RDWR | O_BINARY | O_CREAT, 0644);
   if (fh < 0) {
      sprintf(error, loc("Cannot open file <b>%s</b>"), config_file);
      strcat(error, ": ");
      strcat(error, strerror(errno));
      return 0;
   }

   buf = (char *) xmalloc(strlen(buffer) * 2);
   strlcpy(buf, buffer, strlen(buffer) * 2);
   adjust_crlf(buf, strlen(buffer) * 2);

   i = write(fh, buf, strlen(buf));
   if (i < (int) strlen(buf)) {
      sprintf(error, loc("Cannot write to <b>%s</b>"), config_file);
      strcat(error, ": ");
      strcat(error, strerror(errno));
      close(fh);
      return 0;
   }

   TRUNCATE(fh);

   close(fh);

   /* force re-read of config file */
   check_config_file(TRUE);

   return 1;
}