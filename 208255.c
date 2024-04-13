main_merge_arguments (main_merge_list* merges)
{
  int ret = 0;
  int count = 0;
  main_merge *merge = NULL;
  xd3_stream merge_input;

  if (main_merge_list_empty (merges))
    {
      return 0;
    }

  if ((ret = xd3_config_stream (& merge_input, NULL)) ||
      (ret = xd3_whole_state_init (& merge_input)))
    {
      XPR(NT XD3_LIB_ERRMSG (& merge_input, ret));
      return ret;
    }

  merge = main_merge_list_front (merges);
  while (!main_merge_list_end (merges, merge))
    {
      main_file mfile;
      main_file_init (& mfile);
      mfile.filename = merge->filename;
      mfile.flags = RD_NONEXTERNAL;

      if ((ret = main_file_open (& mfile, merge->filename, XO_READ)))
        {
          goto error;
        }

      ret = main_input (CMD_MERGE_ARG, & mfile, NULL, NULL);

      if (ret == 0)
	{
	  if (count++ == 0)
	    {
	      /* The first merge source is the next merge input. */
	      xd3_swap_whole_state (& recode_stream->whole_target,
				    & merge_input.whole_target);
	    }
	  else
	    {
	      /* Merge the recode_stream with merge_input. */
	      ret = xd3_merge_input_output (recode_stream,
					    & merge_input.whole_target);

	      /* Save the next merge source in merge_input. */
	      xd3_swap_whole_state (& recode_stream->whole_target,
				    & merge_input.whole_target);
	    }
	}

      main_file_cleanup (& mfile);

      if (recode_stream != NULL)
        {
          xd3_free_stream (recode_stream);
          main_free (recode_stream);
          recode_stream = NULL;
        }

      if (main_bdata != NULL)
        {
          main_buffree (main_bdata);
          main_bdata = NULL;
	  main_bsize = 0;
        }

      if (ret != 0)
        {
	  goto error;
        }

      merge = main_merge_list_next (merge);
    }

  XD3_ASSERT (merge_stream == NULL);

  if ((merge_stream = (xd3_stream*) main_malloc (sizeof(xd3_stream))) == NULL)
    {
      ret = ENOMEM;
      goto error;
    }

  if ((ret = xd3_config_stream (merge_stream, NULL)) ||
      (ret = xd3_whole_state_init (merge_stream)))
    {
      XPR(NT XD3_LIB_ERRMSG (& merge_input, ret));
      goto error;
    }

  xd3_swap_whole_state (& merge_stream->whole_target,
			& merge_input.whole_target);
  ret = 0;
 error:
  xd3_free_stream (& merge_input);
  return ret;
}