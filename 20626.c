ModuleExport size_t RegisterGIFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("GIF");
  entry->decoder=(DecodeImageHandler *) ReadGIFImage;
  entry->encoder=(EncodeImageHandler *) WriteGIFImage;
  entry->magick=(IsImageFormatHandler *) IsGIF;
  entry->description=ConstantString("CompuServe graphics interchange format");
  entry->mime_type=ConstantString("image/gif");
  entry->module=ConstantString("GIF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("GIF87");
  entry->decoder=(DecodeImageHandler *) ReadGIFImage;
  entry->encoder=(EncodeImageHandler *) WriteGIFImage;
  entry->magick=(IsImageFormatHandler *) IsGIF;
  entry->adjoin=MagickFalse;
  entry->description=ConstantString("CompuServe graphics interchange format");
  entry->version=ConstantString("version 87a");
  entry->mime_type=ConstantString("image/gif");
  entry->module=ConstantString("GIF");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}