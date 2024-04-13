int my_read(int fh, void *buffer, unsigned int bytes) {
#ifdef OS_UNIX
   int i, n = 0;

   do {
      i = read(fh, (char *) buffer + n, bytes - n);

      /* don't return if an alarm signal was cought */
      if (i == -1 && errno == EINTR)
         continue;

      if (i == -1)
         return -1;

      if (i == 0)
         return n;

      n += i;

   } while (n < (int) bytes);

   return n;
#else
   return read(fh, buffer, bytes);
#endif

   return 0;
}