main_file_open (main_file *xfile, const char* name, int mode)
{
  int ret = 0;

  xfile->mode = mode;

  XD3_ASSERT (name != NULL);
  XD3_ASSERT (! main_file_isopen (xfile));
  if (name[0] == 0)
    {
      XPR(NT "invalid file name: empty string\n");
      return XD3_INVALID;
    }

#if XD3_STDIO
  xfile->file = fopen (name, XOPEN_STDIO);

  ret = (xfile->file == NULL) ? get_errno () : 0;

#elif XD3_POSIX
  /* TODO: Should retry this call if interrupted, similar to read/write */
  if ((ret = open (name, XOPEN_POSIX, XOPEN_MODE)) < 0)
    {
      ret = get_errno ();
    }
  else
    {
      xfile->file = ret;
      ret = 0;
    }

#elif XD3_WIN32
  xfile->file = CreateFile(name,
			   (mode == XO_READ) ? GENERIC_READ : GENERIC_WRITE,
			   FILE_SHARE_READ,
			   NULL,
			   (mode == XO_READ) ?
			   OPEN_EXISTING :
			   (option_force ? CREATE_ALWAYS : CREATE_NEW),
			   FILE_ATTRIBUTE_NORMAL,
			   NULL);
  if (xfile->file == INVALID_HANDLE_VALUE)
    {
      ret = get_errno ();
    }
#endif
  if (ret) { XF_ERROR ("open", name, ret); }
  else     { xfile->realname = name; xfile->nread = 0; }
  return ret;
}