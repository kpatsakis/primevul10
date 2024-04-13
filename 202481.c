loader_init (lt_get_vtable *vtable_func, lt_user_data data)
{
  const lt_dlvtable *vtable = 0;
  int errors = 0;

  if (vtable_func)
    {
      vtable = (*vtable_func) (data);
    }

  /* lt_dlloader_add will LT__SETERROR if it fails.  */
  errors += lt_dlloader_add (vtable);

  assert (errors || vtable);

  if ((!errors) && vtable->dlloader_init)
    {
      if ((*vtable->dlloader_init) (vtable->dlloader_data))
	{
	  LT__SETERROR (INIT_LOADER);
	  ++errors;
	}
    }

  return errors;
}