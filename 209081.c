int msg_compare_reverse_numeric(const void *m1, const void *m2) {
   return ((MSG_LIST *) m2)->number - ((MSG_LIST *) m1)->number;
}