static int* create_offset_array(const pcre_cache_entry* pce,
                                int& size_offsets) {
  /* Allocate memory for the offsets array */
  size_offsets = pce->num_subpats * 3;
  return (int *)req::malloc_noptrs(size_offsets * sizeof(int));
}