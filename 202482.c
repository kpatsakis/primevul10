tryall_dlopen (lt_dlhandle *phandle, const char *filename,
	       lt_dladvise advise, const lt_dlvtable *vtable)
{
  lt_dlhandle	handle		= handles;
  const char *	saved_error	= 0;
  int		errors		= 0;

#ifdef LT_DEBUG_LOADERS
  fprintf (stderr, "tryall_dlopen (%s, %s)\n",
	   filename ? filename : "(null)",
	   vtable ? vtable->name : "(ALL)");
#endif

  LT__GETERROR (saved_error);

  /* check whether the module was already opened */
  for (;handle; handle = handle->next)
    {
      if ((handle->info.filename == filename) /* dlopen self: 0 == 0 */
	  || (handle->info.filename && filename
	      && streq (handle->info.filename, filename)))
	{
	  break;
	}
    }

  if (handle)
    {
      ++handle->info.ref_count;
      *phandle = handle;
      goto done;
    }

  handle = *phandle;
  if (filename)
    {
      /* Comment out the check of file permissions using access.
	 This call seems to always return -1 with error EACCES.
      */
      /* We need to catch missing file errors early so that
	 file_not_found() can detect what happened.
      if (access (filename, R_OK) != 0)
	{
	  LT__SETERROR (FILE_NOT_FOUND);
	  ++errors;
	  goto done;
	} */

      handle->info.filename = lt__strdup (filename);
      if (!handle->info.filename)
	{
	  ++errors;
	  goto done;
	}
    }
  else
    {
      handle->info.filename = 0;
    }

  {
    lt_dlloader loader = lt_dlloader_next (0);
    const lt_dlvtable *loader_vtable;

    do
      {
	if (vtable)
	  loader_vtable = vtable;
	else
	  loader_vtable = lt_dlloader_get (loader);

#ifdef LT_DEBUG_LOADERS
	fprintf (stderr, "Calling %s->module_open (%s)\n",
		 (loader_vtable && loader_vtable->name) ? loader_vtable->name : "(null)",
		 filename ? filename : "(null)");
#endif
	handle->module = (*loader_vtable->module_open) (loader_vtable->dlloader_data,
							filename, advise);
#ifdef LT_DEBUG_LOADERS
	fprintf (stderr, "  Result: %s\n",
		 handle->module ? "Success" : "Failed");
#endif

	if (handle->module != 0)
	  {
	    if (advise)
	      {
		handle->info.is_resident  = advise->is_resident;
		handle->info.is_symglobal = advise->is_symglobal;
		handle->info.is_symlocal  = advise->is_symlocal;
	      }
	    break;
	  }
      }
    while (!vtable && (loader = lt_dlloader_next (loader)));

    /* If VTABLE was given but couldn't open the module, or VTABLE wasn't
       given but we exhausted all loaders without opening the module, bail
       out!  */
    if ((vtable && !handle->module)
	|| (!vtable && !loader))
      {
	FREE (handle->info.filename);
	++errors;
	goto done;
      }

    handle->vtable = loader_vtable;
  }

  LT__SETERRORSTR (saved_error);

 done:
  return errors;
}