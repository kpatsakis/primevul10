ModuleExport size_t RegisterHEICImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("HEIC");
#if defined(MAGICKCORE_HEIC_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadHEICImage;
#if !defined(MAGICKCORE_WINDOWS_SUPPORT)
  entry->encoder=(EncodeImageHandler *) WriteHEICImage;
#endif
#endif
  entry->magick=(IsImageFormatHandler *) IsHEIC;
  entry->description=ConstantString("Apple High efficiency Image Format");
  entry->mime_type=ConstantString("image/x-heic");
  entry->module=ConstantString("GIF");
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->thread_support=MagickFalse;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}