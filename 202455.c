lt_dlinit (void)
{
  int	errors	= 0;

  /* Initialize only at first call. */
  if (++initialized == 1)
    {
      lt__alloc_die	= lt__alloc_die_callback;
      handles		= 0;
      user_search_path	= 0; /* empty search path */

      /* First set up the statically loaded preload module loader, so
	 we can use it to preopen the other loaders we linked in at
	 compile time.  */
      errors += loader_init (get_vtable, 0);

      /* Now open all the preloaded module loaders, so the application
	 can use _them_ to lt_dlopen its own modules.  */
#ifdef HAVE_LIBDLLOADER
      if (!errors)
	{
	  errors += lt_dlpreload (&preloaded_symbols);
	}

      if (!errors)
	{
	  errors += lt_dlpreload_open (LT_STR(LTDLOPEN), loader_init_callback);
	}
#endif /* HAVE_LIBDLLOADER */
    }

#ifdef LT_DEBUG_LOADERS
  lt_dlloader_dump();
#endif

  return errors;
}