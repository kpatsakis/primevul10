ModuleExport size_t RegisterDPXImage(void)
{
  MagickInfo
    *entry;

  static const char
    *DPXNote =
    {
      "Digital Moving Picture Exchange Bitmap, Version 2.0.\n"
      "See SMPTE 268M-2003 specification at http://www.smtpe.org\n"
    };

  entry=SetMagickInfo("DPX");
  entry->decoder=(DecodeImageHandler *) ReadDPXImage;
  entry->encoder=(EncodeImageHandler *) WriteDPXImage;
  entry->magick=(IsImageFormatHandler *) IsDPX;
  entry->seekable_stream=MagickTrue;
  entry->adjoin=MagickFalse;
  entry->description=ConstantString("SMPTE 268M-2003 (DPX 2.0)");
  entry->note=ConstantString(DPXNote);
  entry->module=ConstantString("DPX");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}