print_volume_label (void)
{
  struct tar_stat_info vstat;
  union block vblk;
  enum archive_format dummy;

  memset (&vblk, 0, sizeof (vblk));
  vblk.header.typeflag = GNUTYPE_VOLHDR;
  if (recent_global_header)
    memcpy (vblk.header.mtime, recent_global_header->header.mtime,
	    sizeof vblk.header.mtime);
  tar_stat_init (&vstat);
  assign_string (&vstat.file_name, ".");
  decode_header (&vblk, &vstat, &dummy, 0);
  assign_string (&vstat.file_name, volume_label);
  simple_print_header (&vstat, &vblk, 0);
  tar_stat_destroy (&vstat);
}