lt_dladvise_ext (lt_dladvise *padvise)
{
  assert (padvise && *padvise);
  (*padvise)->try_ext = 1;
  return 0;
}