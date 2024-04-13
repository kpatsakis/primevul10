ModuleExport size_t RegisterTIMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("TIM");
  entry->decoder=(DecodeImageHandler *) ReadTIMImage;
  entry->description=ConstantString("PSX TIM");
  entry->module=ConstantString("TIM");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}