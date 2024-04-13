decode_header (union block *header, struct tar_stat_info *stat_info,
	       enum archive_format *format_pointer, int do_user_group)
{
  enum archive_format format;
  bool hbits;
  mode_t mode = MODE_FROM_HEADER (header->header.mode, &hbits);

  if (strcmp (header->header.magic, TMAGIC) == 0)
    {
      if (header->star_header.prefix[130] == 0
	  && ISOCTAL (header->star_header.atime[0])
	  && header->star_header.atime[11] == ' '
	  && ISOCTAL (header->star_header.ctime[0])
	  && header->star_header.ctime[11] == ' ')
	format = STAR_FORMAT;
      else if (stat_info->xhdr.size)
	format = POSIX_FORMAT;
      else
	format = USTAR_FORMAT;
    }
  else if (strcmp (header->buffer + offsetof (struct posix_header, magic),
		   OLDGNU_MAGIC)
	   == 0)
    format = hbits ? OLDGNU_FORMAT : GNU_FORMAT;
  else
    format = V7_FORMAT;
  *format_pointer = format;

  stat_info->stat.st_mode = mode;
  stat_info->mtime.tv_sec = TIME_FROM_HEADER (header->header.mtime);
  stat_info->mtime.tv_nsec = 0;
  assign_string_n (&stat_info->uname,
		   header->header.uname[0] ? header->header.uname : NULL,
		   sizeof (header->header.uname));
  assign_string_n (&stat_info->gname,
		   header->header.gname[0] ? header->header.gname : NULL,
		   sizeof (header->header.gname));

  xheader_xattr_init (stat_info);

  if (format == OLDGNU_FORMAT && incremental_option)
    {
      stat_info->atime.tv_sec = TIME_FROM_HEADER (header->oldgnu_header.atime);
      stat_info->ctime.tv_sec = TIME_FROM_HEADER (header->oldgnu_header.ctime);
      stat_info->atime.tv_nsec = stat_info->ctime.tv_nsec = 0;
    }
  else if (format == STAR_FORMAT)
    {
      stat_info->atime.tv_sec = TIME_FROM_HEADER (header->star_header.atime);
      stat_info->ctime.tv_sec = TIME_FROM_HEADER (header->star_header.ctime);
      stat_info->atime.tv_nsec = stat_info->ctime.tv_nsec = 0;
    }
  else
    stat_info->atime = stat_info->ctime = start_time;

  if (format == V7_FORMAT)
    {
      stat_info->stat.st_uid = UID_FROM_HEADER (header->header.uid);
      stat_info->stat.st_gid = GID_FROM_HEADER (header->header.gid);
      stat_info->stat.st_rdev = 0;
    }
  else
    {
      if (do_user_group)
	{
	  /* FIXME: Decide if this should somewhat depend on -p.  */

	  if (numeric_owner_option
	      || !*header->header.uname
	      || !uname_to_uid (header->header.uname, &stat_info->stat.st_uid))
	    stat_info->stat.st_uid = UID_FROM_HEADER (header->header.uid);

	  if (numeric_owner_option
	      || !*header->header.gname
	      || !gname_to_gid (header->header.gname, &stat_info->stat.st_gid))
	    stat_info->stat.st_gid = GID_FROM_HEADER (header->header.gid);
	}

      switch (header->header.typeflag)
	{
	case BLKTYPE:
	case CHRTYPE:
	  stat_info->stat.st_rdev =
	    makedev (MAJOR_FROM_HEADER (header->header.devmajor),
		     MINOR_FROM_HEADER (header->header.devminor));
	  break;

	default:
	  stat_info->stat.st_rdev = 0;
	}
    }

  xheader_decode (stat_info);

  if (sparse_member_p (stat_info))
    {
      sparse_fixup_header (stat_info);
      stat_info->is_sparse = true;
    }
  else
    {
      stat_info->is_sparse = false;
      if (((current_format == GNU_FORMAT
	    || current_format == OLDGNU_FORMAT)
	   && current_header->header.typeflag == GNUTYPE_DUMPDIR)
          || stat_info->dumpdir)
	stat_info->is_dumpdir = true;
    }
}