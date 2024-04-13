int eli_compare(const void *e1, const void *e2) {

   if (((EL_INDEX *) e1)->file_time < ((EL_INDEX *) e2)->file_time)
      return -1;
   if (((EL_INDEX *) e1)->file_time >= ((EL_INDEX *) e2)->file_time)
      return 1;
   return 0;
}