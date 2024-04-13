ModuleExport size_t RegisterSUNImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("RAS");
  entry->decoder=(DecodeImageHandler *) ReadSUNImage;
  entry->encoder=(EncodeImageHandler *) WriteSUNImage;
  entry->magick=(IsImageFormatHandler *) IsSUN;
  entry->description=ConstantString("SUN Rasterfile");
  entry->seekable_stream=MagickTrue;
  entry->module=ConstantString("SUN");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("SUN");
  entry->decoder=(DecodeImageHandler *) ReadSUNImage;
  entry->encoder=(EncodeImageHandler *) WriteSUNImage;
  entry->description=ConstantString("SUN Rasterfile");
  entry->seekable_stream=MagickTrue;
  entry->module=ConstantString("SUN");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}