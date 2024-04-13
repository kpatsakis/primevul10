list_archive (void)
{
  off_t block_ordinal = current_block_ordinal ();

  /* Print the header block.  */
  if (verbose_option)
    print_header (&current_stat_info, current_header, block_ordinal);

  if (incremental_option)
    {
      if (verbose_option > 2)
	{
	  if (is_dumpdir (&current_stat_info))
	    list_dumpdir (current_stat_info.dumpdir,
			  dumpdir_size (current_stat_info.dumpdir));
	}
    }

  skip_member ();
}