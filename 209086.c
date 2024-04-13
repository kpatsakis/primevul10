void *xrealloc(void *pointer, size_t bytes) {
   char *temp;
   int old_size;

   /* Align buffer on 4 byte boundery for HP UX and other 64 bit systems to prevent Bus error (core dump) */
   if (bytes & 3)
      bytes += 4 - (bytes & 3);

   if (pointer == NULL)
      return xmalloc(bytes);

   /* check old magic number */
   temp = pointer;
   assert(*((unsigned int *) (temp - 4)) == 0xdeadc0de);
   old_size = *((unsigned int *) (temp - 8));
   assert(*((unsigned int *) (temp + old_size)) == 0xdeadc0de);

   temp = (char *) realloc(temp - 8, bytes + 12);

   if (temp == 0)
      memory_error_and_abort("xrealloc");

   /* put magic number around array */
   *(unsigned int *) (temp + 0) = bytes;
   *(unsigned int *) (temp + 4) = 0xdeadc0de;
   *(unsigned int *) (temp + bytes + 8) = 0xdeadc0de;

   return (temp + 8);
}