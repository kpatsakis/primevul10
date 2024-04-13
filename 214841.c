static struct receiver *cutBackTo(struct receiver **rcvrs, u_int32_t size, u_int32_t max) {
  struct receiver *r, *tmp;
  int i=0;
  int count;

  if(size < max) //return the original table
    return *rcvrs;

  count = size - max;

  HASH_ITER(hh, *rcvrs, r, tmp) {
    if(i++ == count)
      return r;
    HASH_DEL(*rcvrs, r);
    free(r);
  }

  return(NULL);

}