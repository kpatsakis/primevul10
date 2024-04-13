read_and (void (*do_something) (void))
{
  enum read_header status = HEADER_STILL_UNREAD;
  enum read_header prev_status;
  struct timespec mtime;

  base64_init ();
  name_gather ();

  open_archive (ACCESS_READ);
  do
    {
      prev_status = status;
      tar_stat_destroy (&current_stat_info);

      status = read_header (&current_header, &current_stat_info,
                            read_header_auto);
      switch (status)
	{
	case HEADER_STILL_UNREAD:
	case HEADER_SUCCESS_EXTENDED:
	  abort ();

	case HEADER_SUCCESS:

	  /* Valid header.  We should decode next field (mode) first.
	     Ensure incoming names are null terminated.  */
	  decode_header (current_header, &current_stat_info,
			 &current_format, 1);
	  if (! name_match (current_stat_info.file_name)
	      || (TIME_OPTION_INITIALIZED (newer_mtime_option)
		  /* FIXME: We get mtime now, and again later; this causes
		     duplicate diagnostics if header.mtime is bogus.  */
		  && ((mtime.tv_sec
		       = TIME_FROM_HEADER (current_header->header.mtime)),
		      /* FIXME: Grab fractional time stamps from
			 extended header.  */
		      mtime.tv_nsec = 0,
		      current_stat_info.mtime = mtime,
		      OLDER_TAR_STAT_TIME (current_stat_info, m)))
	      || excluded_name (current_stat_info.file_name,
				current_stat_info.parent))
	    {
	      switch (current_header->header.typeflag)
		{
		case GNUTYPE_VOLHDR:
		case GNUTYPE_MULTIVOL:
		  break;

		case DIRTYPE:
		  if (show_omitted_dirs_option)
		    WARN ((0, 0, _("%s: Omitting"),
			   quotearg_colon (current_stat_info.file_name)));
		  FALLTHROUGH;
		default:
		  skip_member ();
		  continue;
		}
	    }

	  transform_stat_info (current_header->header.typeflag,
			       &current_stat_info);
	  (*do_something) ();
	  continue;

	case HEADER_ZERO_BLOCK:
	  if (block_number_option)
	    {
	      char buf[UINTMAX_STRSIZE_BOUND];
	      fprintf (stdlis, _("block %s: ** Block of NULs **\n"),
		       STRINGIFY_BIGINT (current_block_ordinal (), buf));
	    }

	  set_next_block_after (current_header);

	  if (!ignore_zeros_option)
	    {
	      char buf[UINTMAX_STRSIZE_BOUND];

	      status = read_header (&current_header, &current_stat_info,
	                            read_header_auto);
	      if (status == HEADER_ZERO_BLOCK)
		break;
	      WARNOPT (WARN_ALONE_ZERO_BLOCK,
		       (0, 0, _("A lone zero block at %s"),
			STRINGIFY_BIGINT (current_block_ordinal (), buf)));
	      break;
	    }
	  status = prev_status;
	  continue;

	case HEADER_END_OF_FILE:
	  if (block_number_option)
	    {
	      char buf[UINTMAX_STRSIZE_BOUND];
	      fprintf (stdlis, _("block %s: ** End of File **\n"),
		       STRINGIFY_BIGINT (current_block_ordinal (), buf));
	    }
	  break;

	case HEADER_FAILURE:
	  /* If the previous header was good, tell them that we are
	     skipping bad ones.  */
	  set_next_block_after (current_header);
	  switch (prev_status)
	    {
	    case HEADER_STILL_UNREAD:
	      ERROR ((0, 0, _("This does not look like a tar archive")));
	      FALLTHROUGH;
	    case HEADER_ZERO_BLOCK:
	    case HEADER_SUCCESS:
	      if (block_number_option)
		{
		  char buf[UINTMAX_STRSIZE_BOUND];
		  off_t block_ordinal = current_block_ordinal ();
		  block_ordinal -= recent_long_name_blocks;
		  block_ordinal -= recent_long_link_blocks;
		  fprintf (stdlis, _("block %s: "),
			   STRINGIFY_BIGINT (block_ordinal, buf));
		}
	      ERROR ((0, 0, _("Skipping to next header")));
	      break;

	    case HEADER_END_OF_FILE:
	    case HEADER_FAILURE:
	      /* We are in the middle of a cascade of errors.  */
	      break;

	    case HEADER_SUCCESS_EXTENDED:
	      abort ();
	    }
	  continue;
	}
      break;
    }
  while (!all_names_found (&current_stat_info));

  close_archive ();
  names_notfound ();		/* print names not found */
}