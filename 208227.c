main_file_seek (main_file *xfile, xoff_t pos)
{
  int ret = 0;

#if XD3_STDIO
  if (fseek (xfile->file, pos, SEEK_SET) != 0) { ret = get_errno (); }

#elif XD3_POSIX
  if ((xoff_t) lseek (xfile->file, pos, SEEK_SET) != pos)
    { ret = get_errno (); }

#elif XD3_WIN32
# if (_WIN32_WINNT >= 0x0500)
  LARGE_INTEGER move, out;
  move.QuadPart = pos;
  if (SetFilePointerEx(xfile->file, move, &out, FILE_BEGIN) == 0)
    {
      ret = get_errno ();
    }
# else
  if (SetFilePointer(xfile->file, (LONG)pos, NULL, FILE_BEGIN) ==
      INVALID_SET_FILE_POINTER)
    {
      ret = get_errno ();
    }
# endif
#endif

  return ret;
}