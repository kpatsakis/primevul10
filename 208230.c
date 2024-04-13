main_external_compression_cleanup (void)
{
  int i;

  for (i = 0; i < num_subprocs; i += 1)
    {
      if (! ext_subprocs[i]) { continue; }

      kill (ext_subprocs[i], SIGTERM);

      ext_subprocs[i] = 0;
    }
}