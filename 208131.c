main_file_cleanup (main_file *xfile)
{
  XD3_ASSERT (xfile != NULL);

  if (main_file_isopen (xfile))
    {
      main_file_close (xfile);
    }

  if (xfile->snprintf_buf != NULL)
    {
      main_free(xfile->snprintf_buf);
      xfile->snprintf_buf = NULL;
    }

  if (xfile->filename_copy != NULL)
    {
      main_free(xfile->filename_copy);
      xfile->filename_copy = NULL;
    }
}