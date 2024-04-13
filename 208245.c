main_external_compression_finish (void)
{
  int i;
  int ret;

  for (i = 0; i < num_subprocs; i += 1)
    {
      if (! ext_subprocs[i]) { continue; }

      if ((ret = main_waitpid_check (ext_subprocs[i])))
	{
	  return ret;
	}

      ext_subprocs[i] = 0;
    }

  return 0;
}