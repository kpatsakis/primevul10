static void deleteReceivers(struct receiver *rcvrs) {
  struct receiver *current, *tmp;

  HASH_ITER(hh, rcvrs, current, tmp) {
    HASH_DEL(rcvrs, current);
    free(current);
  }
}