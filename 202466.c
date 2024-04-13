argzize_path (const char *path, char **pargz, size_t *pargz_len)
{
  error_t error;

  assert (path);
  assert (pargz);
  assert (pargz_len);

  if ((error = argz_create_sep (path, LT_PATHSEP_CHAR, pargz, pargz_len)))
    {
      switch (error)
	{
	case ENOMEM:
	  LT__SETERROR (NO_MEMORY);
	  break;
	default:
	  LT__SETERROR (UNKNOWN);
	  break;
	}

      return 1;
    }

  return 0;
}