lt_dladvise_local (lt_dladvise *padvise)
{
  assert (padvise && *padvise);
  (*padvise)->is_symlocal = 1;
  return 0;
}