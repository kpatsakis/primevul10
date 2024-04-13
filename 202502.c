lt_dlisresident	(lt_dlhandle handle)
{
  if (!handle)
    {
      LT__SETERROR (INVALID_HANDLE);
      return -1;
    }

  return LT_DLIS_RESIDENT (handle);
}