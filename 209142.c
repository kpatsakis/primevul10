void eputs(const char *buf) {
   char *p;

   p = xmalloc(strlen(buf) + 2);
   strcpy(p, buf);
   strcat(p, "\n");

   (*fputs_handler)(p);

   xfree(p);
}