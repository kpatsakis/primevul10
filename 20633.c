ModuleExport size_t RegisterMTVImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MTV");
  entry->decoder=(DecodeImageHandler *) ReadMTVImage;
  entry->encoder=(EncodeImageHandler *) WriteMTVImage;
  entry->description=ConstantString("MTV Raytracing image format");
  entry->module=ConstantString("MTV");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}