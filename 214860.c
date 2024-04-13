static void updateReceivers(struct receiver **rcvrs, u_int32_t dst_addr,
                            u_int8_t version, u_int32_t num_pkts,
                            struct receiver **topRcvrs) {
  struct receiver *r;
  u_int32_t size;
  int a;

  HASH_FIND_INT(*rcvrs, (int *)&dst_addr, r);
  if(r == NULL) {
    if(((size = HASH_COUNT(*rcvrs)) < MAX_TABLE_SIZE_1)
       || ((a = acceptable(num_pkts)) != 0)){
      r = (struct receiver *)malloc(sizeof(struct receiver));
      if(!r) return;

      r->addr = dst_addr;
      r->version = version;
      r->num_pkts = num_pkts;

      HASH_ADD_INT(*rcvrs, addr, r);

      if((size = HASH_COUNT(*rcvrs)) > MAX_TABLE_SIZE_2){

        HASH_SORT(*rcvrs, receivers_sort_asc);
        *rcvrs = cutBackTo(rcvrs, size, MAX_TABLE_SIZE_1);
        mergeTables(rcvrs, topRcvrs);

        if((size = HASH_COUNT(*topRcvrs)) > MAX_TABLE_SIZE_1){
          HASH_SORT(*topRcvrs, receivers_sort_asc);
          *topRcvrs = cutBackTo(topRcvrs, size, MAX_TABLE_SIZE_1);
        }

        *rcvrs = NULL;
      }
    }
  }
  else
    r->num_pkts += num_pkts;
}