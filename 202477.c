lt_dlerror (void)
{
  const char *error;

  LT__GETERROR (error);
  LT__SETERRORSTR (0);

  return error ? error : NULL;
}