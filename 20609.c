ModuleExport size_t RegisterAAIImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("AAI");
  entry->decoder=(DecodeImageHandler *) ReadAAIImage;
  entry->encoder=(EncodeImageHandler *) WriteAAIImage;
  entry->description=ConstantString("AAI Dune image");
  entry->module=ConstantString("AAI");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}