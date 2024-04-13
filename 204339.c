test_archive_label (void)
{
  base64_init ();
  name_gather ();

  open_archive (ACCESS_READ);
  if (read_header (&current_header, &current_stat_info, read_header_auto)
      == HEADER_SUCCESS)
    {
      decode_header (current_header,
		     &current_stat_info, &current_format, 0);
      if (current_header->header.typeflag == GNUTYPE_VOLHDR)
	ASSIGN_STRING_N (&volume_label, current_header->header.name);

      if (volume_label)
	{
	  if (verbose_option)
	    print_volume_label ();
	  if (!name_match (volume_label) && multi_volume_option)
	    {
	      char *s = drop_volume_label_suffix (volume_label);
	      name_match (s);
	      free (s);
	    }
	}
    }
  close_archive ();
  label_notfound ();
}