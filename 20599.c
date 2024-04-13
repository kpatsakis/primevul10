ModuleExport size_t RegisterPIXImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PIX");
  entry->decoder=(DecodeImageHandler *) ReadPIXImage;
  entry->description=ConstantString("Alias/Wavefront RLE image format");
  entry->module=ConstantString("PIX");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}