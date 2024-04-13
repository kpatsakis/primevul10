main_file_read (main_file  *ifile,
		uint8_t    *buf,
		size_t      size,
		size_t     *nread,
		const char *msg)
{
  int ret = 0;

#if XD3_STDIO
  size_t result;

  result = fread (buf, 1, size, ifile->file);

  if (result < size && ferror (ifile->file))
    {
      ret = get_errno ();
    }
  else
    {
      *nread = result;
    }

#elif XD3_POSIX
  ret = xd3_posix_io (ifile->file, buf, size, (xd3_posix_func*) &read, nread);
#elif XD3_WIN32
  ret = xd3_win32_io (ifile->file, buf, size, 1 /* is_read */, nread);
#endif

  if (ret)
    {
      XPR(NT "%s: %s: %s\n", msg, ifile->filename, xd3_mainerror (ret));
    }
  else
    {
      if (option_verbose > 4) { XPR(NT "read %s: %zu bytes\n",
				    ifile->filename, (*nread)); }
      ifile->nread += (*nread);
    }

  return ret;
}