lt_dlforeachfile (const char *search_path,
		  int (*func) (const char *filename, void *data),
		  void *data)
{
  int is_done = 0;
  file_worker_func **fpptr = &func;

  if (search_path)
    {
      /* If a specific path was passed, search only the directories
	 listed in it.  */
      is_done = foreach_dirinpath (search_path, 0,
				   foreachfile_callback, fpptr, data);
    }
  else
    {
      /* Otherwise search the default paths.  */
      is_done = foreach_dirinpath (user_search_path, 0,
				   foreachfile_callback, fpptr, data);
      if (!is_done)
	{
	  is_done = foreach_dirinpath (getenv(LTDL_SEARCHPATH_VAR), 0,
				       foreachfile_callback, fpptr, data);
	}

#if defined(LT_MODULE_PATH_VAR)
      if (!is_done)
	{
	  is_done = foreach_dirinpath (getenv(LT_MODULE_PATH_VAR), 0,
				       foreachfile_callback, fpptr, data);
	}
#endif
#if defined(LT_DLSEARCH_PATH)
      if (!is_done && *sys_dlsearch_path)
	{
	  is_done = foreach_dirinpath (sys_dlsearch_path, 0,
				       foreachfile_callback, fpptr, data);
	}
#endif
    }

  return is_done;
}