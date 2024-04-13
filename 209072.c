int check_smtp_error(char *str, int expected, char *error, int error_size) {
   if (atoi(str) != expected) {
      if (error)
         strlcpy(error, str + 4, error_size);
      return 0;
   }

   return 1;
}