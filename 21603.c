sh_exit (s)
     int s;
{
#if defined (MALLOC_DEBUG) && defined (USING_BASH_MALLOC)
  if (malloc_trace_at_exit && (subshell_environment & (SUBSHELL_COMSUB|SUBSHELL_PROCSUB)) == 0)
    trace_malloc_stats (get_name_for_error (), (char *)NULL);
  /* mlocation_write_table (); */
#endif

  exit (s);
}