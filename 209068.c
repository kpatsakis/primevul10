const char *find_section(const char *buf, const char *name) {
   const char *pstart;
   char *pstr, str[80];

   do {
      if (*buf == '[') {
         pstart = buf;
         buf++;
         pstr = str;
         while (*buf && *buf != ']' && *buf != '\n' && *buf != '\r')
            *pstr++ = *buf++;
         *pstr = 0;
         if (strieq(str, name))
            return pstart;
      }

      if (buf)
         buf = strchr(buf, '\n');
      if (buf)
         buf++;
      if (buf && *buf == '\r')
         buf++;

   } while (buf);

   return NULL;
}