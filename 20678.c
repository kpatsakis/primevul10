ModuleExport size_t RegisterIPLImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("IPL");
  entry->decoder=(DecodeImageHandler *) ReadIPLImage;
  entry->encoder=(EncodeImageHandler *) WriteIPLImage;
  entry->magick=(IsImageFormatHandler *) IsIPL;
  entry->adjoin=MagickTrue;
  entry->description=ConstantString("IPL Image Sequence");
  entry->module=ConstantString("IPL");
  entry->endian_support=MagickTrue;
  entry->seekable_stream=MagickTrue;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}