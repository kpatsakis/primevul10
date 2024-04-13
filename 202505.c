lt_argz_insertinorder (char **pargz, size_t *pargz_len, const char *entry)
{
  char *before = 0;

  assert (pargz);
  assert (pargz_len);
  assert (entry && *entry);

  if (*pargz)
    while ((before = argz_next (*pargz, *pargz_len, before)))
      {
	int cmp = strcmp (entry, before);

	if (cmp < 0)  break;
	if (cmp == 0) return 0;	/* No duplicates! */
      }

  return lt_argz_insert (pargz, pargz_len, before, entry);
}