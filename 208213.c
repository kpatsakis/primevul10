main_file_stat (main_file *xfile, xoff_t *size)
{
  int ret = 0;
#if XD3_WIN32
  if (GetFileType(xfile->file) != FILE_TYPE_DISK)
    {
      return -1;
    }
# if (_WIN32_WINNT >= 0x0500)
  {
    LARGE_INTEGER li;
    if (GetFileSizeEx(xfile->file, &li) == 0)
      {
	return get_errno ();
      }
    *size = li.QuadPart;
  }
# else
  {
    DWORD filesize = GetFileSize(xfile->file, NULL);
    if (filesize == INVALID_FILE_SIZE)
      {
	return get_errno ()
      }
    *size = filesize;
  }
# endif
#else
  struct stat sbuf;
  if (fstat (XFNO (xfile), & sbuf) < 0)
    {
      ret = get_errno ();
      return ret;
    }

  if (! S_ISREG (sbuf.st_mode))
    {
      return ESPIPE;
    }
  (*size) = sbuf.st_size;
#endif
  return ret;
}