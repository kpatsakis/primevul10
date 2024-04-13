ModuleExport size_t RegisterYUVImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("YUV");
  entry->decoder=(DecodeImageHandler *) ReadYUVImage;
  entry->encoder=(EncodeImageHandler *) WriteYUVImage;
  entry->adjoin=MagickFalse;
  entry->raw=MagickTrue;
  entry->description=ConstantString("CCIR 601 4:1:1 or 4:2:2");
  entry->module=ConstantString("YUV");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}