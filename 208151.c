main_file_init (main_file *xfile)
{
  memset (xfile, 0, sizeof (*xfile));

#if XD3_POSIX
  xfile->file = -1;
#endif
#if XD3_WIN32
  xfile->file = INVALID_HANDLE_VALUE;
#endif
}