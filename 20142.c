ModuleExport size_t RegisterWMFImage(void)
{
  MagickInfo
    *entry;

  entry = AcquireMagickInfo("WMF","WMZ","Compressed Windows Meta File");
#if defined(MAGICKCORE_SANS_DELEGATE) || defined(MAGICKCORE_WMF_DELEGATE)
  entry->decoder=ReadWMFImage;
#endif
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("WMF","WMF","Windows Meta File");
#if defined(MAGICKCORE_SANS_DELEGATE) || defined(MAGICKCORE_WMF_DELEGATE)
  entry->decoder=ReadWMFImage;
#endif
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}