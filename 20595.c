ModuleExport void UnregisterRAWImage(void)
{
  (void) UnregisterMagickInfo("R");
  (void) UnregisterMagickInfo("C");
  (void) UnregisterMagickInfo("G");
  (void) UnregisterMagickInfo("M");
  (void) UnregisterMagickInfo("B");
  (void) UnregisterMagickInfo("Y");
  (void) UnregisterMagickInfo("A");
  (void) UnregisterMagickInfo("O");
  (void) UnregisterMagickInfo("K");
}