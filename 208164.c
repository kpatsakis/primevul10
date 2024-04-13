main_malloc (size_t size)
{
  void *r = main_malloc1 (size);
  if (r) { IF_DEBUG (main_mallocs += 1); }
  return r;
}