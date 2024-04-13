ModuleExport size_t RegisterICONImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CUR");
  entry->decoder=(DecodeImageHandler *) ReadICONImage;
  entry->encoder=(EncodeImageHandler *) WriteICONImage;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Microsoft icon");
  entry->module=ConstantString("ICON");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("ICO");
  entry->decoder=(DecodeImageHandler *) ReadICONImage;
  entry->encoder=(EncodeImageHandler *) WriteICONImage;
  entry->adjoin=MagickTrue;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Microsoft icon");
  entry->module=ConstantString("ICON");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("ICON");
  entry->decoder=(DecodeImageHandler *) ReadICONImage;
  entry->encoder=(EncodeImageHandler *) WriteICONImage;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Microsoft icon");
  entry->module=ConstantString("ICON");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}