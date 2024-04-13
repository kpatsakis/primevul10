main_file_exists (main_file *xfile)
{
  struct stat sbuf;
  return stat (xfile->filename, & sbuf) == 0 && S_ISREG (sbuf.st_mode);
}