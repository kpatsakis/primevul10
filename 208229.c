main_get_winsize (main_file *ifile) {
  xoff_t file_size = 0;
  usize_t size = option_winsize;
  static shortbuf iszbuf;

  if (main_file_stat (ifile, &file_size) == 0)
    {
      size = (usize_t) min(file_size, (xoff_t) size);
    }

  size = max(size, XD3_ALLOCSIZE);

  if (option_verbose > 1)
    {
      XPR(NT "input %s window size %s\n",
	  ifile->filename,
	  main_format_bcnt (size, &iszbuf));
    }

  return size;
}