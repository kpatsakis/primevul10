int msg_compare(const void *m1, const void *m2) {
   return strcoll(((MSG_LIST *) m1)->string, ((MSG_LIST *) m2)->string);
}