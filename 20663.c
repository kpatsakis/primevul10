ModuleExport size_t RegisterAVSImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("AVS");
  entry->decoder=(DecodeImageHandler *) ReadAVSImage;
  entry->encoder=(EncodeImageHandler *) WriteAVSImage;
  entry->description=ConstantString("AVS X image");
  entry->module=ConstantString("AVS");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}