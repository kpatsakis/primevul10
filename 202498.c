unload_deplibs (lt_dlhandle handle)
{
  int i;
  int errors = 0;
  lt_dlhandle cur = handle;

  if (cur->depcount)
    {
      for (i = 0; i < cur->depcount; ++i)
	{
	  if (!LT_DLIS_RESIDENT (cur->deplibs[i]))
	    {
	      errors += lt_dlclose (cur->deplibs[i]);
	    }
	}
      FREE (cur->deplibs);
    }

  return errors;
}