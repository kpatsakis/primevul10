main_file_isopen (main_file *xfile)
{
#if XD3_STDIO
  return xfile->file != NULL;

#elif XD3_POSIX
  return xfile->file != -1;

#elif XD3_WIN32
  return xfile->file != INVALID_HANDLE_VALUE;
#endif
}