main_file_close (main_file *xfile)
{
  int ret = 0;

  if (! main_file_isopen (xfile))
    {
      return 0;
    }

#if XD3_STDIO
  ret = fclose (xfile->file);
  xfile->file = NULL;

#elif XD3_POSIX
  ret = close (xfile->file);
  xfile->file = -1;

#elif XD3_WIN32
  if (!CloseHandle(xfile->file)) {
    ret = get_errno ();
  }
  xfile->file = INVALID_HANDLE_VALUE;
#endif

  if (ret != 0) { XF_ERROR ("close", xfile->filename, ret = get_errno ()); }
  return ret;
}