ModuleExport void UnregisterWMFImage(void)
{
  (void) UnregisterMagickInfo("WMZ");
  (void) UnregisterMagickInfo("WMF");
}