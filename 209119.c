void rsprintf(const char *format, ...) {
   va_list argptr;
   char str[10000];

   va_start(argptr, format);
   vsprintf(str, (char *) format, argptr);
   va_end(argptr);

   if (strlen_retbuf + (int) strlen(str) + 1 >= return_buffer_size) {
      return_buffer = xrealloc(return_buffer, return_buffer_size + 100000);
      memset(return_buffer + return_buffer_size, 0, 100000);
      return_buffer_size += 100000;
   }

   strcpy(return_buffer + strlen_retbuf, str);

   strlen_retbuf += strlen(str);
}