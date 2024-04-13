Fseek (FILE *stream, file_offset offset, int ptrname)
{
  if (file_seek (stream, offset, ptrname) != 0)
    pfatal ("fseek");
}