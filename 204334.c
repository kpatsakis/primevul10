skip_member (void)
{
  if (!current_stat_info.skipped)
    {
      char save_typeflag = current_header->header.typeflag;
      set_next_block_after (current_header);

      mv_begin_read (&current_stat_info);

      if (current_stat_info.is_sparse)
	sparse_skip_file (&current_stat_info);
      else if (save_typeflag != DIRTYPE)
	skip_file (current_stat_info.stat.st_size);

      mv_end ();
    }
}