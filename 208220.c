main_alloc (void   *opaque,
	    size_t  items,
	    usize_t  size)
{
  return main_malloc1 (items * size);
}