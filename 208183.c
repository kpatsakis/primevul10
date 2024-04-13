main_cleanup (void)
{
  if (appheader_used != NULL &&
      appheader_used != option_appheader)
    {
      main_free (appheader_used);
      appheader_used = NULL;
    }

  main_buffree (main_bdata);
  main_bdata = NULL;
  main_bsize = 0;

  main_lru_cleanup();

  if (recode_stream != NULL)
    {
      xd3_free_stream (recode_stream);
      main_free (recode_stream);
      recode_stream = NULL;
    }

  if (merge_stream != NULL)
    {
      xd3_free_stream (merge_stream);
      main_free (merge_stream);
      merge_stream = NULL;
    }

  XD3_ASSERT (main_mallocs == 0);
}