void rsputs(const char *str) {
   if (strlen_retbuf + (int) strlen(str) + 1 >= return_buffer_size) {
      return_buffer = xrealloc(return_buffer, return_buffer_size + (int) strlen(str) + 100000);
      memset(return_buffer + return_buffer_size, 0, (int) strlen(str) + 100000);
      return_buffer_size += (int) strlen(str) + 100000;
   }

   strcpy(return_buffer + strlen_retbuf, str);
   strlen_retbuf += strlen(str);
}