ModuleExport size_t RegisterRGBImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("RGB");
  entry->decoder=(DecodeImageHandler *) ReadRGBImage;
  entry->encoder=(EncodeImageHandler *) WriteRGBImage;
  entry->raw=MagickTrue;
  entry->endian_support=MagickTrue;
  entry->description=ConstantString("Raw red, green, and blue samples");
  entry->module=ConstantString("RGB");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("RGBA");
  entry->decoder=(DecodeImageHandler *) ReadRGBImage;
  entry->encoder=(EncodeImageHandler *) WriteRGBImage;
  entry->raw=MagickTrue;
  entry->endian_support=MagickTrue;
  entry->description=ConstantString("Raw red, green, blue, and alpha samples");
  entry->module=ConstantString("RGB");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("RGBO");
  entry->decoder=(DecodeImageHandler *) ReadRGBImage;
  entry->encoder=(EncodeImageHandler *) WriteRGBImage;
  entry->raw=MagickTrue;
  entry->endian_support=MagickTrue;
  entry->description=ConstantString("Raw red, green, blue, and opacity samples");
  entry->module=ConstantString("RGB");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}