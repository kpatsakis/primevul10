lt_dlgetinfo (lt_dlhandle handle)
{
  if (!handle)
    {
      LT__SETERROR (INVALID_HANDLE);
      return 0;
    }

  return &(handle->info);
}