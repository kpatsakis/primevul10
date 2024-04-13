ModuleExport size_t RegisterFITSImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("FITS");
  entry->decoder=(DecodeImageHandler *) ReadFITSImage;
  entry->encoder=(EncodeImageHandler *) WriteFITSImage;
  entry->magick=(IsImageFormatHandler *) IsFITS;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Flexible Image Transport System");
  entry->module=ConstantString("FITS");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("FTS");
  entry->decoder=(DecodeImageHandler *) ReadFITSImage;
  entry->encoder=(EncodeImageHandler *) WriteFITSImage;
  entry->magick=(IsImageFormatHandler *) IsFITS;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Flexible Image Transport System");
  entry->module=ConstantString("FITS");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}