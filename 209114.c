int msg_compare_reverse(const void *m1, const void *m2) {
   return strcoll(((MSG_LIST *) m2)->string, ((MSG_LIST *) m1)->string);
}