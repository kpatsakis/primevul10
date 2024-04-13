int xvasprintf(char **ptr, const char *format, va_list ap) {
   int n;
   va_list save;

#ifdef va_copy
   va_copy(save, ap);
#else
                                                                                                                           #ifdef __va_copy
   __va_copy(save, ap);
#else
   save = ap;
#endif
#endif

   n = vasprintf(ptr, format, save);

   if (n == -1 || !*ptr) {
      printf("Not enough memory");
      exit(EXIT_FAILURE);
   }

   return n;
}