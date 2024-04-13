lt_dlexit (void)
{
  /* shut down libltdl */
  lt_dlloader *loader   = 0;
  lt_dlhandle  handle   = handles;
  int	       errors   = 0;

  if (!initialized)
    {
      LT__SETERROR (SHUTDOWN);
      ++errors;
      goto done;
    }

  /* shut down only at last call. */
  if (--initialized == 0)
    {
      int	level;

      while (handles && LT_DLIS_RESIDENT (handles))
	{
	  handles = handles->next;
	}

      /* close all modules */
      for (level = 1; handle; ++level)
	{
	  lt_dlhandle cur = handles;
	  int saw_nonresident = 0;

	  while (cur)
	    {
	      lt_dlhandle tmp = cur;
	      cur = cur->next;
	      if (!LT_DLIS_RESIDENT (tmp))
		{
		  saw_nonresident = 1;
		  if (tmp->info.ref_count <= level)
		    {
		      if (lt_dlclose (tmp))
			{
			  ++errors;
			}
		      /* Make sure that the handle pointed to by 'cur' still exists.
			 lt_dlclose recursively closes dependent libraries which removes
			 them from the linked list.  One of these might be the one
			 pointed to by 'cur'.  */
		      if (cur)
			{
			  for (tmp = handles; tmp; tmp = tmp->next)
			    if (tmp == cur)
			      break;
			  if (! tmp)
			    cur = handles;
			}
		    }
		}
	    }
	  /* done if only resident modules are left */
	  if (!saw_nonresident)
	    break;
	}

      /* When removing loaders, we can only find out failure by testing
	 the error string, so avoid a spurious one from an earlier
	 failed command. */
      if (!errors)
	LT__SETERRORSTR (0);

      /* close all loaders */
      for (loader = (lt_dlloader *) lt_dlloader_next (NULL); loader;)
	{
	  lt_dlloader *next   = (lt_dlloader *) lt_dlloader_next (loader);
	  lt_dlvtable *vtable = (lt_dlvtable *) lt_dlloader_get (loader);

	  if ((vtable = lt_dlloader_remove ((char *) vtable->name)))
	    {
	      FREE (vtable);
	    }
	  else
	    {
	      /* ignore errors due to resident modules */
	      const char *err;
	      LT__GETERROR (err);
	      if (err)
		++errors;
	    }

	  loader = next;
	}

      FREE(user_search_path);
    }

 done:
  return errors;
}