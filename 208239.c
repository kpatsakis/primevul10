main_malloc1 (size_t size)
{
  void* r = malloc (size);
  if (r == NULL) { XPR(NT "malloc: %s\n", xd3_mainerror (ENOMEM)); }
  return r;
}