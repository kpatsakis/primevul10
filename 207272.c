my_bitmap_init_memroot(MY_BITMAP *map, uint n_bits, MEM_ROOT *mem_root)
{
  my_bitmap_map *bitmap_buf;

  if (!(bitmap_buf= (my_bitmap_map*) alloc_root(mem_root,
                                                bitmap_buffer_size(n_bits))) ||
      my_bitmap_init(map, bitmap_buf, n_bits, FALSE))
    return TRUE;
  bitmap_clear_all(map);
  return FALSE;
}