lt_dladvise_global (lt_dladvise *padvise)
{
  assert (padvise && *padvise);
  (*padvise)->is_symglobal = 1;
  return 0;
}