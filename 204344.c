print_for_mkdir (char *dirname, int length, mode_t mode)
{
  char modes[11];

  if (verbose_option > 1)
    {
      /* File type and modes.  */

      modes[0] = 'd';
      pax_decode_mode (mode, modes + 1);

      if (block_number_option)
	{
	  char buf[UINTMAX_STRSIZE_BOUND];
	  fprintf (stdlis, _("block %s: "),
		   STRINGIFY_BIGINT (current_block_ordinal (), buf));
	}

      fprintf (stdlis, "%s %*s %s\n", modes, ugswidth + 1 + datewidth,
	       _("Creating directory:"), quotearg (dirname));
    }
}