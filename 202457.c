lt_dladvise_resident (lt_dladvise *padvise)
{
  assert (padvise && *padvise);
  (*padvise)->is_resident = 1;
  return 0;
}