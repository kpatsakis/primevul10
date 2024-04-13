main_file_write (main_file *ofile, uint8_t *buf, usize_t size, const char *msg)
{
  int ret = 0;

#if XD3_STDIO
  usize_t result;

  result = fwrite (buf, 1, size, ofile->file);

  if (result != size) { ret = get_errno (); }

#elif XD3_POSIX
  ret = xd3_posix_io (ofile->file, buf, size, (xd3_posix_func*) &write, NULL);

#elif XD3_WIN32
  ret = xd3_win32_io (ofile->file, buf, size, 0, NULL);

#endif

  if (ret)
    {
      XPR(NT "%s: %s: %s\n", msg, ofile->filename, xd3_mainerror (ret));
    }
  else
    {
      if (option_verbose > 5) { XPR(NT "write %s: %u bytes\n",
				    ofile->filename, size); }
      ofile->nwrite += size;
    }

  return ret;
}