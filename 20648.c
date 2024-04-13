ModuleExport void UnregisterFITSImage(void)
{
  (void) UnregisterMagickInfo("FITS");
  (void) UnregisterMagickInfo("FTS");
}