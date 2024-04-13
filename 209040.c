void xfree(void *pointer) {
   char *temp;
   int old_size;

   if (!pointer)
      return;

   /* check for magic byte */
   temp = pointer;
   assert(*((unsigned int *) (temp - 4)) == 0xdeadc0de);
   old_size = *((unsigned int *) (temp - 8));
   assert(*((unsigned int *) (temp + old_size)) == 0xdeadc0de);

   free(temp - 8);
}