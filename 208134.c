main_print_vcdiff_file (main_file *xfile, main_file *file, const char *type)
{
  int ret;  /* Used by above macros */
  if (file->filename)
    {
      VC(UT "XDELTA filename (%s):     %s\n", type,
	 file->filename)VE;
    }
  if (file->compressor)
    {
      VC(UT "XDELTA ext comp (%s):     %s\n", type,
	 file->compressor->recomp_cmdname)VE;
    }
  return 0;
}