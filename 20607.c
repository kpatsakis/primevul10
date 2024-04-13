ModuleExport size_t RegisterFLIFImage(void)
{
  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
  entry=SetMagickInfo("FLIF");
#if defined(MAGICKCORE_FLIF_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadFLIFImage;
  entry->encoder=(EncodeImageHandler *) WriteFLIFImage;
  (void) FormatLocaleString(version,MaxTextExtent,"libflif %d.%d.%d [%04X]",
    (FLIF_VERSION >> 16) & 0xff,
    (FLIF_VERSION  >> 8) & 0xff,
    (FLIF_VERSION  >> 0) & 0xff,FLIF_ABI_VERSION);
#endif
  entry->description=ConstantString("Free Lossless Image Format");
  entry->adjoin=MagickTrue;
  entry->module=ConstantString("FLIF");
  entry->mime_type=ConstantString("image/flif");
  entry->magick=(IsImageFormatHandler *) IsFLIF;
  if (*version != '\0')
    entry->version=ConstantString(version);
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}