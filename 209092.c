const char *find_next_section(const char *buf) {
   do {
      if (*buf == '[')
         return buf;

      if (buf)
         buf = strchr(buf, '\n');
      if (buf)
         buf++;
   } while (buf);

   return NULL;
}