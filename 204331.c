simple_print_header (struct tar_stat_info *st, union block *blk,
		     off_t block_ordinal)
{
  char modes[12];
  char const *time_stamp;
  int time_stamp_len;
  char *temp_name;

  /* These hold formatted ints.  */
  char uform[max (INT_BUFSIZE_BOUND (intmax_t), UINTMAX_STRSIZE_BOUND)];
  char gform[sizeof uform];
  char *user, *group;
  char size[2 * UINTMAX_STRSIZE_BOUND];
  				/* holds formatted size or major,minor */
  char uintbuf[UINTMAX_STRSIZE_BOUND];
  int pad;
  int sizelen;

  if (show_transformed_names_option)
    temp_name = st->file_name ? st->file_name : st->orig_file_name;
  else
    temp_name = st->orig_file_name ? st->orig_file_name : st->file_name;

  if (block_number_option)
    {
      char buf[UINTMAX_STRSIZE_BOUND];
      if (block_ordinal < 0)
	block_ordinal = current_block_ordinal ();
      block_ordinal -= recent_long_name_blocks;
      block_ordinal -= recent_long_link_blocks;
      fprintf (stdlis, _("block %s: "),
	       STRINGIFY_BIGINT (block_ordinal, buf));
    }

  if (verbose_option <= 1)
    {
      /* Just the fax, mam.  */
      fputs (quotearg (temp_name), stdlis);
      if (show_transformed_names_option && st->had_trailing_slash)
	fputc ('/', stdlis);
      fputc ('\n', stdlis);
    }
  else
    {
      /* File type and modes.  */

      modes[0] = '?';
      switch (blk->header.typeflag)
	{
	case GNUTYPE_VOLHDR:
	  volume_label_printed = true;
	  modes[0] = 'V';
	  break;

	case GNUTYPE_MULTIVOL:
	  modes[0] = 'M';
	  break;

	case GNUTYPE_LONGNAME:
	case GNUTYPE_LONGLINK:
	  modes[0] = 'L';
	  ERROR ((0, 0, _("Unexpected long name header")));
	  break;

	case GNUTYPE_SPARSE:
	case REGTYPE:
	case AREGTYPE:
	  modes[0] = st->had_trailing_slash ? 'd' : '-';
	  break;
	case LNKTYPE:
	  modes[0] = 'h';
	  break;
	case GNUTYPE_DUMPDIR:
	  modes[0] = 'd';
	  break;
	case DIRTYPE:
	  modes[0] = 'd';
	  break;
	case SYMTYPE:
	  modes[0] = 'l';
	  break;
	case BLKTYPE:
	  modes[0] = 'b';
	  break;
	case CHRTYPE:
	  modes[0] = 'c';
	  break;
	case FIFOTYPE:
	  modes[0] = 'p';
	  break;
	case CONTTYPE:
	  modes[0] = 'C';
	  break;
	}

      pax_decode_mode (st->stat.st_mode, modes + 1);

      /* extended attributes:  GNU `ls -l'-like preview */
      xattrs_print_char (st, modes + 10);

      /* Time stamp.  */

      time_stamp = tartime (st->mtime, full_time_option);
      time_stamp_len = strlen (time_stamp);
      if (datewidth < time_stamp_len)
	datewidth = time_stamp_len;

      /* User and group names.  */

      if (st->uname
	  && st->uname[0]
	  && current_format != V7_FORMAT
	  && !numeric_owner_option)
	user = st->uname;
      else
	user = STRINGIFY_BIGINT (st->stat.st_uid, uform);

      if (st->gname
	  && st->gname[0]
	  && current_format != V7_FORMAT
	  && !numeric_owner_option)
	group = st->gname;
      else
	group = STRINGIFY_BIGINT (st->stat.st_gid, gform);

      /* Format the file size or major/minor device numbers.  */

      switch (blk->header.typeflag)
	{
	case CHRTYPE:
	case BLKTYPE:
	  strcpy (size,
		  STRINGIFY_BIGINT (major (st->stat.st_rdev), uintbuf));
	  strcat (size, ",");
	  strcat (size,
		  STRINGIFY_BIGINT (minor (st->stat.st_rdev), uintbuf));
	  break;

	default:
	  /* st->stat.st_size keeps stored file size */
	  strcpy (size, STRINGIFY_BIGINT (st->stat.st_size, uintbuf));
	  break;
	}

      /* Figure out padding and print the whole line.  */

      sizelen = strlen (size);
      pad = strlen (user) + 1 + strlen (group) + 1 + sizelen;
      if (pad > ugswidth)
	ugswidth = pad;

      fprintf (stdlis, "%s %s/%s %*s %-*s",
	       modes, user, group, ugswidth - pad + sizelen, size,
	       datewidth, time_stamp);

      fprintf (stdlis, " %s", quotearg (temp_name));
      if (show_transformed_names_option && st->had_trailing_slash)
	fputc ('/', stdlis);

      switch (blk->header.typeflag)
	{
	case SYMTYPE:
	  fprintf (stdlis, " -> %s\n", quotearg (st->link_name));
	  break;

	case LNKTYPE:
	  fprintf (stdlis, _(" link to %s\n"), quotearg (st->link_name));
	  break;

	default:
	  {
	    char type_string[2];
	    type_string[0] = blk->header.typeflag;
	    type_string[1] = '\0';
	    fprintf (stdlis, _(" unknown file type %s\n"),
		     quote (type_string));
	  }
	  break;

	case AREGTYPE:
	case REGTYPE:
	case GNUTYPE_SPARSE:
	case CHRTYPE:
	case BLKTYPE:
	case DIRTYPE:
	case FIFOTYPE:
	case CONTTYPE:
	case GNUTYPE_DUMPDIR:
	  putc ('\n', stdlis);
	  break;

	case GNUTYPE_LONGLINK:
	  fprintf (stdlis, _("--Long Link--\n"));
	  break;

	case GNUTYPE_LONGNAME:
	  fprintf (stdlis, _("--Long Name--\n"));
	  break;

	case GNUTYPE_VOLHDR:
	  fprintf (stdlis, _("--Volume Header--\n"));
	  break;

	case GNUTYPE_MULTIVOL:
	  strcpy (size,
		  STRINGIFY_BIGINT
		  (UINTMAX_FROM_HEADER (blk->oldgnu_header.offset),
		   uintbuf));
	  fprintf (stdlis, _("--Continued at byte %s--\n"), size);
	  break;
	}
    }
  fflush (stdlis);
  xattrs_print (st);
}