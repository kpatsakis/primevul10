skip_file (off_t size)
{
  union block *x;

  /* FIXME: Make sure mv_begin_read is always called before it */

  if (seekable_archive)
    {
      off_t nblk = seek_archive (size);
      if (nblk >= 0)
	size -= nblk * BLOCKSIZE;
      else
	seekable_archive = false;
    }

  mv_size_left (size);

  while (size > 0)
    {
      x = find_next_block ();
      if (! x)
	FATAL_ERROR ((0, 0, _("Unexpected EOF in archive")));

      set_next_block_after (x);
      size -= BLOCKSIZE;
      mv_size_left (size);
    }
}