zalloc (voidpf opaque, unsigned int items, unsigned int size)
{
  (void) opaque;
  return (voidpf) xcalloc (items, size);
}