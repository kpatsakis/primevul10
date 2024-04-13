ModuleExport void UnregisterRGBImage(void)
{
  (void) UnregisterMagickInfo("RGBO");
  (void) UnregisterMagickInfo("RGBA");
  (void) UnregisterMagickInfo("RGB");
}