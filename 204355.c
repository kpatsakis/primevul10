print_header (struct tar_stat_info *st, union block *blk,
	      off_t block_ordinal)
{
  if (current_format == POSIX_FORMAT && !volume_label_printed && volume_label)
    {
      print_volume_label ();
      volume_label_printed = true;
    }

  simple_print_header (st, blk, block_ordinal);
}