lt_dladvise_destroy (lt_dladvise *padvise)
{
  if (padvise)
    FREE(*padvise);
  return 0;
}