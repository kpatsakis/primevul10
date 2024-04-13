lt_dladvise_preload (lt_dladvise *padvise)
{
  assert (padvise && *padvise);
  (*padvise)->try_preload_only = 1;
  return 0;
}