lt_dlmakeresident (lt_dlhandle handle)
{
  int errors = 0;

  if (!handle)
    {
      LT__SETERROR (INVALID_HANDLE);
      ++errors;
    }
  else
    {
      handle->info.is_resident = 1;
    }

  return errors;
}