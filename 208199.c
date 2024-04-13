main_ident_compressor (const char *ident)
{
  usize_t i;

  for (i = 0; i < SIZEOF_ARRAY (extcomp_types); i += 1)
    {
      if (strcmp (extcomp_types[i].ident, ident) == 0)
	{
	  return & extcomp_types[i];
	}
    }

  return NULL;
}